#include "wow_window_words_binding.h"
#include "wow_window_creation_binding.h"
#include "wow_dialog_creation_binding.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_runtime.h"
#include "wow_user_object_bindings.h"
#include "wow_task_order_bindings.h"
#include "wow_menu_native_bindings.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

#include <stdio.h>

/* DIVERGENCE(ADAPTER-WOW-027): original kernel WND/thread locks retain WW
 * across callbacks, even if a callback destroys the window. Public HWND does
 * not expose original WW storage. Keep exactly that storage in a native
 * property, with borrowed references; no window/class/guest policy lives here.
 * One worker per process; the lock covers only property/ref publication. */
struct wow_window_words_binding {
    LONG references;
    BOOL enrolled;
    BOOL terminal_pending;
    wow_window_creation_menu_state creation_menu;
    WW words;
    wow_class_words_binding *class_words;
    wow_cleanup_window cleanup;
    wow_window_callback callback;
    /* Public USER owns this opaque CallProc identity. Original WND cleanup
     * must see a native forwarding address, never its FFFF/WOW-looking tag. */
    WNDPROC native_callproc;
    wow_user_object_table *objects;
    ULONG retired_guest_backing;
};
static SRWLOCK publication_lock = SRWLOCK_INIT;
static const WCHAR property_name[] = L"NTVDM.WOW.WindowWords";

LRESULT CALLBACK wow_window_call_native_previous(HWND window, UINT message,
    WPARAM wp, LPARAM lp);

static DWORD logical_procedure(wow_window_words_binding *binding)
{
    DWORD procedure = (DWORD)InterlockedCompareExchange(
        (LONG *)&binding->cleanup.lpfnWndProc, 0, 0);
    return procedure == (DWORD)wow_window_call_native_previous ?
        (DWORD)binding->native_callproc : procedure;
}

static BOOL own_window(HWND window)
{
    /* Original GetWindowData(GWL_WNDPROC) enforces TestWindowProcess.
     * Native USER keeps that check and accepts original short HWNDs, unlike
     * GetWindowThreadProcessId. Validate before reading the private property. */
    return GetWindowLongA(window, GWL_WNDPROC) != 0;
}

BOOL WINAPI wow_window_words_attach(HWND window, const WW *words)
{
    return wow_window_words_attach_class(window, words, NULL);
}

wow_window_words_binding *WINAPI wow_window_words_allocate(void)
{
    wow_window_words_binding *binding = HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY, sizeof(*binding));
    if (!binding) SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    else binding->references = 1;
    return binding;
}

BOOL WINAPI wow_window_words_attach_class(HWND window, const WW *words,
    wow_class_words_binding *class_words)
{
    return wow_window_words_attach_target(window, words, class_words, NULL);
}

BOOL WINAPI wow_window_words_attach_target(HWND window, const WW *words,
    wow_class_words_binding *class_words, const wow_window_dispatch_target *target)
{
    wow_window_words_binding *binding;
    BOOL attached = FALSE;
    BOOL observed;
    DWORD error;
    if (!words) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (target && (!target->procedure ||
            target->procedure == (DWORD)wow_window_native_proc ||
            target->procedure == (DWORD)wow_window_call_native_previous ||
            (HIWORD(target->procedure) != WNDPROC_HANDLE &&
             (target->procedure & WNDPROC_WOW) && !target->callback))) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!own_window(window)) return FALSE;
    binding = wow_user_borrow_observed_binding(window);
    observed = binding != NULL;
    if (!binding) binding = wow_window_words_allocate();
    if (!binding) return FALSE;
    AcquireSRWLockExclusive(&publication_lock);
    if (binding->enrolled || GetPropW(window, property_name))
        SetLastError(ERROR_ALREADY_EXISTS);
    else attached = SetPropW(window, property_name, binding);
    error = GetLastError();
    if (attached) {
        if (observed) InterlockedIncrement(&binding->references); /* Property. */
        binding->enrolled = TRUE;
        binding->words = *words;
        ZeroMemory(&binding->creation_menu, sizeof(binding->creation_menu));
        binding->creation_menu.checked = TRUE; /* Not necessarily a new HWND. */
        binding->class_words = class_words;
        ZeroMemory(&binding->cleanup, sizeof(binding->cleanup));
        binding->cleanup.storage = binding;
        binding->cleanup.pcls = wow_class_words_entry(class_words);
        binding->cleanup.adwWOW = binding->words.adwWOW;
        binding->cleanup.lpfnWndProc = target ? (WNDPROC)target->procedure : NULL;
        binding->native_callproc = NULL;
        if (target && HIWORD(target->procedure) == WNDPROC_HANDLE) {
            binding->native_callproc = (WNDPROC)target->procedure;
            binding->cleanup.lpfnWndProc = wow_window_call_native_previous;
        }
        binding->callback = target ? target->callback : NULL;
        binding->objects = NULL;
        binding->retired_guest_backing = 0u;
        wow_class_words_window_association(class_words, TRUE);
    }
    ReleaseSRWLockExclusive(&publication_lock);
    if (!attached && !observed) wow_window_words_release(binding);
    SetLastError(error);
    return attached;
}

wow_window_words_binding *WINAPI wow_window_words_acquire(HWND window)
{
    wow_window_words_binding *binding;
    if (!own_window(window)) return NULL;
    AcquireSRWLockShared(&publication_lock);
    binding = (wow_window_words_binding *)GetPropW(window, property_name);
    if (binding) InterlockedIncrement(&binding->references);
    ReleaseSRWLockShared(&publication_lock);
    if (!binding) SetLastError(ERROR_NOT_FOUND);
    return binding;
}

WW *WINAPI wow_window_words_value(wow_window_words_binding *binding)
{
    return binding ? &binding->words : NULL;
}

void WINAPI wow_window_words_begin_creation(wow_window_words_binding *binding,
    HMENU menu)
{
    ZeroMemory(&binding->creation_menu, sizeof(binding->creation_menu));
    binding->creation_menu.input_menu = menu;
}

static BOOL prepare_creation_menu(HWND window, UINT message, LPARAM lp,
    wow_window_words_binding *binding)
{
    if (message == WM_GETMINMAXINFO && !binding->creation_menu.checked) {
        CREATESTRUCTA creation = {0};
        /* Original createw.c associates the menu before sizing callbacks.
         * Retain the caller's explicit menu even before USER exposes it. */
        creation.style = binding->words.dwStyle;
        creation.hMenu = binding->creation_menu.input_menu;
        return wow_window_creation_menu(window, &creation, binding->cleanup.pcls,
            &binding->creation_menu);
    }
    if (message == WM_NCCREATE || message == WM_CREATE)
        return wow_window_creation_menu(window, (CREATESTRUCTA *)lp,
            binding->cleanup.pcls, &binding->creation_menu);
    return TRUE;
}

BOOL WINAPI wow_window_words_refresh(HWND window, wow_window_words_binding *binding)
{
    WINDOWINFO info = { sizeof(info) };
    LONG instance;
    DWORD saved = GetLastError();
    if (!binding) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!GetWindowInfo(window, &info)) return FALSE;
    SetLastError(ERROR_SUCCESS);
    instance = GetWindowLongA(window, GWL_HINSTANCE);
    if (!instance && GetLastError() != ERROR_SUCCESS) return FALSE;
    AcquireSRWLockExclusive(&publication_lock);
    if (GetPropW(window, property_name) != binding) {
        ReleaseSRWLockExclusive(&publication_lock);
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return FALSE;
    }
    /* wowuserp.h aliases these live WND fields after the three WOW words.
     * Preserve cached classification and guest procedures during refresh. */
    binding->words.dwStyle = info.dwStyle;
    binding->words.dwExStyle = info.dwExStyle;
    binding->words.hInstance = (HANDLE)(ULONG_PTR)(DWORD)instance;
    ReleaseSRWLockExclusive(&publication_lock);
    SetLastError(saved);
    return TRUE;
}

wow_cleanup_window *WINAPI wow_window_words_cleanup_value(wow_window_words_binding *binding)
{
    return binding ? &binding->cleanup : NULL;
}

BOOL WINAPI wow_window_words_exchange(HWND window, unsigned index,
    DWORD value, DWORD *previous)
{
    wow_window_words_binding *binding;
    ULONG backing = 0, ignored;
    BOOL result = FALSE;
    if (index >= WND_CNT_WOWDWORDS || !previous) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    AcquireSRWLockExclusive(&publication_lock);
    if (GetPropW(window, property_name) != binding) goto done;
    if (binding->objects) {
        ULONG full = (ULONG)(ULONG_PTR)binding->objects->windows[LOWORD(window)];
        backing = mvdm_softpc_wow_page_domain_borrow_window(full);
        if (!backing || !mvdm_softpc_wow_page_domain_exchange_window_word(
                backing, index, value, &ignored)) goto done;
    }
    *previous = (DWORD)InterlockedExchange((LONG *)&binding->words.adwWOW[index], value);
    result = TRUE;
done:
    /* A live published WND cannot be freed by this temporary release. */
    if (backing) (void)mvdm_softpc_wow_page_domain_release_window(backing);
    ReleaseSRWLockExclusive(&publication_lock);
    wow_window_words_release(binding);
    if (!result) SetLastError(ERROR_INVALID_STATE);
    return result;
}

BOOL WINAPI wow_window_words_publish_owner(HWND window, wow_window_words_binding *binding,
    wow_task_order_thread *thread)
{
    BOOL result=FALSE;
    if (!binding || !thread || !thread->ppi || !thread->ppi->objects) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    AcquireSRWLockExclusive(&publication_lock);
    if (GetPropW(window,property_name)!=binding || binding->objects) {
        SetLastError(ERROR_INVALID_STATE);
    } else if (wow_user_window_publish(thread->ppi->objects,window,&binding->cleanup,thread)) {
        binding->cleanup.thread=thread;
        binding->objects=thread->ppi->objects;
        InterlockedIncrement(&binding->references); /* Published table owner. */
        result=TRUE;
    }
    ReleaseSRWLockExclusive(&publication_lock);
    return result;
}

void WINAPI wow_window_words_release(wow_window_words_binding *binding)
{
    if (binding && InterlockedDecrement(&binding->references) == 0) {
        /* Like HMMarkObjectDestroy/ThreadUnlock, the final existing native
         * borrow owns physical WND release. A terminated worker has already
         * discarded its entire guest address space. */
        if (binding->retired_guest_backing &&
                mvdm_softpc_wow_page_domain_active())
            (void)mvdm_softpc_wow_page_domain_release_retained_window(
                binding->retired_guest_backing);
        wow_class_words_release(binding->class_words);
        HeapFree(GetProcessHeap(), 0, binding);
    }
}

DWORD *WINAPI wow_window_class_words_value(wow_window_words_binding *binding)
{
    return binding ? wow_class_words_value(binding->class_words) : NULL;
}

BOOL WINAPI wow_window_class_procedure_read(HWND window, DWORD *procedure)
{
    wow_window_words_binding *binding;
    wow_window_dispatch_target target;
    BOOL found = FALSE;
    if (!procedure) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    if (wow_class_words_target(binding->class_words, &target)) {
        *procedure = target.procedure; found = TRUE;
    }
    wow_window_words_release(binding);
    if (!found) SetLastError(ERROR_NOT_FOUND);
    return found;
}

BOOL WINAPI wow_window_class_procedure_exchange(HWND window, DWORD procedure,
    DWORD *previous)
{
    wow_window_words_binding *binding;
    BOOL result;
    if (procedure == (DWORD)wow_window_native_proc) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    result = wow_class_words_exchange_procedure(binding->class_words,
        procedure, previous);
    wow_window_words_release(binding);
    return result;
}

static BOOL detach_matching(HWND window, wow_window_words_binding *expected)
{
    wow_window_words_binding *binding;
    BOOL table_reference;
    if (!own_window(window)) return FALSE;
    AcquireSRWLockExclusive(&publication_lock);
    binding = (wow_window_words_binding *)GetPropW(window, property_name);
    if (!binding || (expected && binding != expected)) {
        ReleaseSRWLockExclusive(&publication_lock);
        return FALSE;
    }
    table_reference = binding->objects != NULL;
    if (binding->objects && !wow_user_window_retire(binding->objects, window,
            &binding->cleanup, &binding->retired_guest_backing)) {
        ReleaseSRWLockExclusive(&publication_lock);
        return FALSE;
    }
    binding = (wow_window_words_binding *)RemovePropW(window, property_name);
    if (binding) {
        binding->enrolled = FALSE;
        if (binding->objects) {
            binding->objects=NULL;
        }
        wow_class_words_window_association(binding->class_words, FALSE);
        binding->cleanup.thread = NULL; /* Retired borrows no longer own a thread. */
    }
    ReleaseSRWLockExclusive(&publication_lock);
    if (table_reference) wow_window_words_release(binding);
    wow_window_words_release(binding);
    return binding != NULL;
}

void WINAPI wow_window_words_detach(HWND window)
{
    detach_matching(window, NULL);
}

BOOL WINAPI wow_window_words_detach_if(HWND window, wow_window_words_binding *expected)
{
    if (!expected) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    return detach_matching(window, expected);
}

BOOL WINAPI wow_window_words_terminal(HWND window, wow_window_words_binding *binding)
{
    BOOL retired = TRUE, property_reference = FALSE, table_reference = FALSE;
    if (!binding) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    AcquireSRWLockExclusive(&publication_lock);
    if (!binding->enrolled && !binding->terminal_pending) {
        ReleaseSRWLockExclusive(&publication_lock);
        return TRUE; /* Nested destruction already consumed this owner. */
    }
    table_reference = binding->objects != NULL;
    if (table_reference)
        retired = wow_user_window_retire(binding->objects, window,
            &binding->cleanup, &binding->retired_guest_backing);
    if (binding->enrolled) {
        if (GetPropW(window, property_name) == binding)
            RemovePropW(window, property_name);
        binding->enrolled = FALSE;
        property_reference = TRUE;
    }
    binding->terminal_pending = !retired;
    if (retired) {
        binding->objects = NULL;
        binding->cleanup.thread = NULL;
        wow_class_words_window_association(binding->class_words, FALSE);
    }
    ReleaseSRWLockExclusive(&publication_lock);
    if (retired && table_reference) wow_window_words_release(binding);
    if (property_reference) wow_window_words_release(binding);
    return retired;
}

BOOL WINAPI wow_window_words_collect_retired(wow_user_object_table *objects)
{
    ULONG i;
    BOOL complete = TRUE;
    if (!objects) return TRUE;
    for (i = 1u; i <= objects->last_handle; ++i) {
        wow_window_words_binding *binding = NULL;
        HWND window = NULL;
        AcquireSRWLockShared(&publication_lock);
        if (objects->entries[i].bType == TYPE_WINDOW) {
            wow_cleanup_window *view = objects->entries[i].phead;
            if (view && view->storage && view->storage->terminal_pending) {
                binding = view->storage;
                window = objects->windows[i];
                InterlockedIncrement(&binding->references);
            }
        }
        ReleaseSRWLockShared(&publication_lock);
        if (binding) {
            if (!wow_window_words_terminal(window, binding)) complete = FALSE;
            wow_window_words_release(binding);
        }
    }
    return complete;
}

void WINAPI wow_window_note_native_procedure(HWND window, WNDPROC procedure,
    BOOL unicode)
{
    DWORD error = GetLastError();
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_window_words_binding *binding;
    BOOL held;
    BOOL matched = FALSE;
    WORD fnid;
    WNDPROC canonical;
    if (!runtime || !runtime->thread) return;
    binding = wow_window_words_acquire(window);
    if (!binding) { SetLastError(error); return; }
    held = runtime->exclusive_held;
    if (!held && !wow_user_runtime_enter(runtime))
        wow_user_native_call_failed("record-native-cleanup-owner", GetLastError());
    if (runtime->thread->ppi)
        matched = wow_class_words_match_native(runtime->thread->ppi->classes,
            procedure, unicode, &fnid, &canonical);
    /* GetClassInfo's process lookup need not identify the native class
     * attached to this HWND. For an imported native registration, compare
     * the procedure actually being called with this window's class target,
     * not with a name or the current process-wide lookup winner. Logical
     * application classes are excluded; cross-class standard calls above
     * still select the called standard target, as PatchThreadWindows needs. */
    if (!matched && wow_class_words_native_registration(binding->class_words)) {
        WNDPROC actual = (WNDPROC)(unicode ? GetClassLongW(window, GCL_WNDPROC) :
            GetClassLongA(window, GCL_WNDPROC));
        if (actual && actual == procedure) {
            canonical = (WNDPROC)GetClassLongA(window, GCL_WNDPROC);
            fnid = wow_class_words_entry(binding->class_words)->fnid;
            matched = canonical != NULL && fnid != 0;
        }
    }
    if (matched) {
        binding->cleanup.fnid = fnid;
        binding->cleanup.native_cleanup = canonical;
    }
    if (!held && !wow_user_runtime_leave(runtime))
        wow_user_native_call_failed("release-native-cleanup-owner", GetLastError());
    wow_window_words_release(binding);
    SetLastError(error);
}

BOOL WINAPI wow_window_words_prepare_exit(HWND window,
    wow_window_words_binding *binding)
{
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    /* Preserve the original cleanup boundary's invalid-HWND diagnosis.
     * A disappeared native object is not a lock/owner-state failure. */
    if (!IsWindow(window)) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    if (!runtime || !runtime->exclusive_held || !runtime->thread || !binding ||
            binding->cleanup.thread != runtime->thread ||
            !(runtime->thread->TIF_flags & WOW_TASK_TIF_INCLEANUP) ||
            GetPropW(window, property_name) != binding) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* createw.c PatchThreadWindows chooses the actually exercised standard
     * target, not the creation class or an arbitrary application subclass.
     * Public USER retains native control state; use its existing target via
     * the CPD forwarder rather than interpreting a procedure token here. */
    binding->native_callproc = binding->cleanup.native_cleanup ?
        binding->cleanup.native_cleanup : DefWindowProcA;
    binding->cleanup.lpfnWndProc = wow_window_call_native_previous;
    binding->cleanup.serverSideProc = TRUE;
    binding->callback = NULL;
    wow_dialog_creation_prepare_exit(window, binding);
    return TRUE;
}

static BOOL dispatch_bound(HWND window, UINT message, WPARAM wp,
    LPARAM lp, DWORD procedure, wow_window_callback callback, LONG *result,
    BOOL terminal_owner)
{
    wow_window_words_binding *binding;
    wow_window_dispatch_view view;
    wow_user_borrow_scope scope;
    wow_task_callback_scope task_scope;
    ULONG saved_callback[2];
    BOOL callback_published = FALSE;
    if (!result || !procedure || HIWORD(procedure) == WNDPROC_HANDLE ||
            ((procedure & WNDPROC_WOW) && !callback)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    view.window = window;
    view.adwWOW = binding->words.adwWOW;
    view.callback = callback;
    /* Original client.c dispatches native procedures without entering WOW.
     * Keep the window alive, but do not make a native callback wait for the
     * guest CPU or publish a guest callback window it does not consume. */
    if (!(procedure & WNDPROC_WOW)) {
        __try {
            if (!prepare_creation_menu(window, message, lp, binding)) return FALSE;
            wow_window_note_native_procedure(window, (WNDPROC)procedure, FALSE);
            *result = DispatchClientMessage(&view, message, wp, lp, procedure);
        } __finally {
            DWORD error = GetLastError();
            if (terminal_owner && message == WM_NCDESTROY)
                (void)wow_window_words_terminal(window, binding);
            wow_window_words_release(binding);
            SetLastError(error);
        }
        return TRUE;
    }
    if (!wow_task_callback_enter(&task_scope)) {
        DWORD error = GetLastError();
        wow_window_words_release(binding); SetLastError(error); return FALSE;
    }
    if (!wow_user_borrow_enter(&scope)) {
        DWORD error = GetLastError();
        (void)wow_task_callback_leave(&task_scope);
        wow_window_words_release(binding); SetLastError(error); return FALSE;
    }
    __try {
        /* Resource conversion may enter the guest. Establish the same task
         * and alias scope as the callback before loading the class menu. */
        if (!prepare_creation_menu(window, message, lp, binding)) return FALSE;
        /* Original wowuserp.h WW aliases WND's live style/ExStyle/hModule;
         * rtl/getset.c exposes those same fields through public indices.
         * Refresh before dispatch, preserving all WOW-owned words and flags.
         * This is not a replacement for direct-access/callback scope binding. */
        if (!wow_window_words_refresh(window, binding)) return FALSE;
        if (binding->objects) {
            if (!wow_user_window_refresh_message(window, message, wp, lp)) return FALSE;
            if (!mvdm_softpc_wow_page_domain_callback_window(
                    (unsigned long)(ULONG_PTR)window, saved_callback)) return FALSE;
            callback_published = TRUE;
            /* Native creation/SetMenu installs the menu before calling the
             * client (original createw.c/mnapi.c ordering). USER16 GetMenu
             * reads WND/MENU directly inside that callback, so publishing
             * only after the native API returns is too late. A failed view
             * refresh must not dispatch this callback with stale fields. */
            if (!wow_menu_native_refresh_window(window)) return FALSE;
            if (!wow_menu_native_refresh_message(message, wp, lp)) return FALSE;
        }
        *result = DispatchClientMessage(&view, message, wp, lp, procedure);
    } __finally {
        DWORD error = GetLastError();
        if (terminal_owner && message == WM_NCDESTROY)
            (void)wow_window_words_terminal(window, binding);
        if (callback_published &&
                !mvdm_softpc_wow_page_domain_restore_callback(saved_callback))
            wow_user_native_call_failed("restore-window-callback-cache", ERROR_INVALID_STATE);
        /* Release this WND owner while the alias/task scope is still live.
         * Its final retained backing can then be reclaimed by scope leave,
         * including the sampled CLS it kept alive during nested destruction. */
        wow_window_words_release(binding);
        wow_user_borrow_leave_required(&scope);
        (void)wow_task_callback_leave(&task_scope);
        SetLastError(error);
    }
    return TRUE;
}

BOOL WINAPI wow_window_dispatch_bound(HWND window, UINT message, WPARAM wp,
    LPARAM lp, DWORD procedure, wow_window_callback callback, LONG *result)
{
    return dispatch_bound(window, message, wp, lp, procedure, callback, result, FALSE);
}

BOOL WINAPI wow_window_procedure_read(HWND window, DWORD *procedure)
{
    wow_window_words_binding *binding;
    if (!procedure) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    AcquireSRWLockShared(&publication_lock);
    *procedure = logical_procedure(binding);
    ReleaseSRWLockShared(&publication_lock);
    wow_window_words_release(binding);
    return TRUE;
}

BOOL WINAPI wow_window_procedure_exchange(HWND window, DWORD procedure, DWORD *previous)
{
    wow_window_words_binding *binding;
    /* Original xxxSetWindowData resolves CPD before WND publication. Public
     * USER exposes no resolver; retain its token separately and let the
     * existing original native dispatch call the finite forwarding entry. */
    if (!previous || !procedure || procedure == (DWORD)wow_window_call_native_previous ||
            procedure == (DWORD)wow_window_native_proc) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    if (HIWORD(procedure) != WNDPROC_HANDLE &&
            (procedure & WNDPROC_WOW) && !binding->callback) {
        wow_window_words_release(binding);
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    AcquireSRWLockExclusive(&publication_lock);
    *previous = logical_procedure(binding);
    if (HIWORD(procedure) == WNDPROC_HANDLE) {
        binding->native_callproc = (WNDPROC)procedure;
        procedure = (DWORD)wow_window_call_native_previous;
    }
    InterlockedExchange((LONG *)&binding->cleanup.lpfnWndProc, (LONG)procedure);
    ReleaseSRWLockExclusive(&publication_lock);
    wow_window_words_release(binding);
    return TRUE;
}

LRESULT CALLBACK wow_window_call_native_previous(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    wow_window_words_binding *binding = wow_window_words_acquire(window);
    LRESULT result = 0;
    if (!binding) return 0;
    __try {
        WNDPROC previous;
        AcquireSRWLockShared(&publication_lock);
        previous = binding->native_callproc;
        ReleaseSRWLockShared(&publication_lock);
        if (!previous) SetLastError(ERROR_INVALID_STATE);
        else {
            wow_window_note_native_procedure(window, previous, FALSE);
            result = CallWindowProcA(previous, window, message, wp, lp);
        }
    } __finally {
        wow_window_words_release(binding);
    }
    return result;
}

/* The product witness is deliberately off by default.  It observes the
 * native entrance and the already-completed original dispatch separately so
 * an integration test can distinguish USER delivery from WOW callback work.
 * It neither retains the HWND nor affects message ownership. */
static VOID window_trace(HWND window, UINT message, LONG result,
    const char *stage, DWORD error)
{
    char path[MAX_PATH], line[176];
    DWORD bytes, written;
    HANDLE file;
    LONG native_procedure;

    if (!GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", path,
            sizeof(path))) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    native_procedure = GetWindowLongA(window, GWL_WNDPROC);
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu %s hwnd=%08lX message=%04X native=%08lX result=%08lX tid=%lu error=%lu\r\n",
        (unsigned long)GetCurrentProcessId(), stage,
        (unsigned long)(ULONG_PTR)window, (unsigned)message,
        (unsigned long)native_procedure, (unsigned long)result,
        (unsigned long)GetCurrentThreadId(), (unsigned long)error);
    if (bytes) (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}

/* Native ABI entrance only: original DispatchClientMessage owns selection,
 * and original W32Win16WndProcEx owns decoding/thunking/CallBack16. */
LRESULT CALLBACK wow_window_native_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    wow_window_words_binding *binding = wow_window_words_acquire(window);
    LONG result = message == WM_CREATE ? -1 : 0;
    DWORD saved = GetLastError();
    BOOL dispatched = FALSE;

    window_trace(window, message, 0, "WindowEntry", saved);
    SetLastError(saved);
    if (!binding) {
        if (message == WM_NCDESTROY) return DefWindowProcA(window, message, wp, lp);
        window_trace(window, message, result, "WindowRejected", ERROR_INVALID_DATA);
        SetLastError(ERROR_INVALID_DATA);
        return result;
    }
    __try {
        dispatched = dispatch_bound(window, message, wp, lp,
            (DWORD)InterlockedCompareExchange((LONG *)&binding->cleanup.lpfnWndProc, 0, 0),
            binding->callback, &result, TRUE);
    } __finally {
        saved = GetLastError();
        /* Also cover denied callback entry. The table retains any deferred
         * withdrawal; normal dispatch already retired before task leave. */
        if (message == WM_NCDESTROY) (void)wow_window_words_terminal(window, binding);
        wow_window_words_release(binding);
        SetLastError(saved);
    }
    /* Default-off production witness: observe the original dispatch result
     * after the call has already completed, without changing its callback or
     * destruction ordering. */
    saved = GetLastError();
    window_trace(window, message, result,
        dispatched ? "WindowDispatch" : "WindowRejected", saved);
    SetLastError(saved);
    return result;
}
