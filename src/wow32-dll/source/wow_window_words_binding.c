#include "wow_window_words_binding.h"
#include "wow_window_creation_binding.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_object_bindings.h"
#include "wow_task_order_bindings.h"
#include "wow_menu_native_bindings.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

#include <stdio.h>

typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

/* DIVERGENCE(ADAPTER-WOW-027): original kernel WND/thread locks retain WW
 * across callbacks, even if a callback destroys the window. Public HWND does
 * not expose original WW storage. Keep exactly that storage in a native
 * property, with borrowed references; no window/class/guest policy lives here.
 * One worker per process; the lock covers only property/ref publication. */
struct wow_window_words_binding {
    LONG references;
    BOOL enrolled;
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
        binding->class_words = class_words;
        ZeroMemory(&binding->cleanup, sizeof(binding->cleanup));
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
    if (!own_window(window)) return FALSE;
    AcquireSRWLockExclusive(&publication_lock);
    binding = (wow_window_words_binding *)GetPropW(window, property_name);
    if (!binding || (expected && binding != expected)) {
        ReleaseSRWLockExclusive(&publication_lock);
        return FALSE;
    }
    if (binding->objects && !wow_user_window_retire(binding->objects, window,
            &binding->cleanup, &binding->retired_guest_backing)) {
        ReleaseSRWLockExclusive(&publication_lock);
        return FALSE;
    }
    binding = (wow_window_words_binding *)RemovePropW(window, property_name);
    if (binding) {
        if (binding->objects) {
            binding->objects=NULL;
        }
        wow_class_words_window_association(binding->class_words, FALSE);
        binding->cleanup.thread = NULL; /* Retired borrows no longer own a thread. */
    }
    ReleaseSRWLockExclusive(&publication_lock);
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

BOOL WINAPI wow_window_dispatch_bound(HWND window, UINT message, WPARAM wp,
    LPARAM lp, DWORD procedure, wow_window_callback callback, LONG *result)
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
            *result = DispatchClientMessage(&view, message, wp, lp, procedure);
        } __finally {
            wow_window_words_release(binding);
        }
        return TRUE;
    }
    if (!wow_task_callback_enter(&task_scope)) {
        wow_window_words_release(binding); return FALSE;
    }
    if (!wow_user_borrow_enter(&scope)) {
        (void)wow_task_callback_leave(&task_scope);
        wow_window_words_release(binding); return FALSE;
    }
    __try {
        /* Original wowuserp.h WW aliases WND's live style/ExStyle/hModule;
         * rtl/getset.c exposes those same fields through public indices.
         * Refresh before dispatch, preserving all WOW-owned words and flags.
         * This is not a replacement for direct-access/callback scope binding. */
        if (!wow_window_words_refresh(window, binding)) return FALSE;
        if (binding->objects) {
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
        if (callback_published)
            (void)mvdm_softpc_wow_page_domain_restore_callback(saved_callback);
        wow_user_borrow_leave(&scope);
        (void)wow_task_callback_leave(&task_scope);
        wow_window_words_release(binding);
    }
    return TRUE;
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
        else result = CallWindowProcA(previous, window, message, wp, lp);
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
    const char *stage)
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
        "%lu %s hwnd=%08lX message=%04X native=%08lX result=%08lX tid=%lu\r\n",
        (unsigned long)GetCurrentProcessId(), stage,
        (unsigned long)(ULONG_PTR)window, (unsigned)message,
        (unsigned long)native_procedure, (unsigned long)result,
        (unsigned long)GetCurrentThreadId());
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

    window_trace(window, message, 0, "WindowEntry");
    SetLastError(saved);
    if (!binding) {
        if (message == WM_NCDESTROY) return DefWindowProcA(window, message, wp, lp);
        SetLastError(ERROR_INVALID_DATA);
        return result;
    }
    __try {
        if ((message == WM_NCCREATE || message == WM_CREATE) &&
                !wow_window_creation_menu(window, (CREATESTRUCTA *)lp))
            __leave;
        wow_window_dispatch_bound(window, message, wp, lp,
            (DWORD)InterlockedCompareExchange((LONG *)&binding->cleanup.lpfnWndProc, 0, 0),
            binding->callback, &result);
    } __finally {
        if (message == WM_NCDESTROY) wow_window_words_detach_if(window, binding);
        wow_window_words_release(binding);
    }
    /* Default-off production witness: observe the original dispatch result
     * after the call has already completed, without changing its callback or
     * destruction ordering. */
    saved = GetLastError();
    window_trace(window, message, result, "WindowDispatch");
    SetLastError(saved);
    return result;
}
