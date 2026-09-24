#include "wow_window_words_binding.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_object_bindings.h"
#include "wow_task_order_bindings.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

#include <stdio.h>

extern LONG __cdecl VdmFreeVirtualMemory(ULONG address);

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
    WW words;
    wow_class_words_binding *class_words;
    wow_cleanup_window cleanup;
    wow_window_callback callback;
    wow_user_object_table *objects;
    ULONG retired_guest_backing;
};
static SRWLOCK publication_lock = SRWLOCK_INIT;
static const WCHAR property_name[] = L"NTVDM.WOW.WindowWords";

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
    DWORD error;
    if (!words) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (target && (!target->procedure ||
            target->procedure == (DWORD)wow_window_native_proc ||
            HIWORD(target->procedure) == WNDPROC_HANDLE ||
            ((target->procedure & WNDPROC_WOW) && !target->callback))) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!own_window(window)) return FALSE;
    binding = HeapAlloc(GetProcessHeap(), 0, sizeof(*binding));
    if (!binding) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    binding->references = 1; /* Property ownership. */
    binding->words = *words;
    binding->class_words = class_words;
    ZeroMemory(&binding->cleanup, sizeof(binding->cleanup));
    binding->cleanup.pcls = wow_class_words_entry(class_words);
    binding->cleanup.lpfnWndProc = target ? (WNDPROC)target->procedure : NULL;
    binding->callback = target ? target->callback : NULL;
    binding->objects = NULL;
    binding->retired_guest_backing = 0u;
    AcquireSRWLockExclusive(&publication_lock);
    if (GetPropW(window, property_name)) SetLastError(ERROR_ALREADY_EXISTS);
    else attached = SetPropW(window, property_name, binding);
    error = GetLastError();
    if (attached) wow_class_words_window_association(class_words, TRUE);
    ReleaseSRWLockExclusive(&publication_lock);
    if (!attached) HeapFree(GetProcessHeap(), 0, binding);
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

wow_cleanup_window *WINAPI wow_window_words_cleanup_value(wow_window_words_binding *binding)
{
    return binding ? &binding->cleanup : NULL;
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
            (void)VdmFreeVirtualMemory(binding->retired_guest_backing);
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
    ULONG saved_callback[2];
    BOOL callback_published = FALSE;
    if (!result || !procedure || HIWORD(procedure) == WNDPROC_HANDLE ||
            ((procedure & WNDPROC_WOW) && !callback)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    if (!wow_user_borrow_enter(&scope)) {
        wow_window_words_release(binding); return FALSE;
    }
    view.window = window;
    view.adwWOW = binding->words.adwWOW;
    view.callback = callback;
    __try {
        WINDOWINFO info = { sizeof(info) };
        LONG instance;
        /* Original wowuserp.h WW aliases WND's live style/ExStyle/hModule;
         * rtl/getset.c exposes those same fields through public indices.
         * Refresh before dispatch, preserving all WOW-owned words and flags.
         * This is not a replacement for direct-access/callback scope binding. */
        if (!GetWindowInfo(window, &info)) return FALSE;
        SetLastError(ERROR_SUCCESS);
        instance = GetWindowLongA(window, GWL_HINSTANCE);
        if (!instance && GetLastError() != ERROR_SUCCESS) return FALSE;
        binding->words.dwStyle = info.dwStyle;
        binding->words.dwExStyle = info.dwExStyle;
        binding->words.hInstance = (HANDLE)(ULONG_PTR)(DWORD)instance;
        if (binding->objects) {
            if (!mvdm_softpc_wow_page_domain_callback_window(
                    (unsigned long)(ULONG_PTR)window, saved_callback)) return FALSE;
            callback_published = TRUE;
        }
        *result = DispatchClientMessage(&view, message, wp, lp, procedure);
    } __finally {
        if (callback_published)
            (void)mvdm_softpc_wow_page_domain_restore_callback(saved_callback);
        wow_user_borrow_leave(&scope);
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
    *procedure = (DWORD)InterlockedCompareExchange((LONG *)&binding->cleanup.lpfnWndProc, 0, 0);
    wow_window_words_release(binding);
    return TRUE;
}

BOOL WINAPI wow_window_procedure_exchange(HWND window, DWORD procedure, DWORD *previous)
{
    wow_window_words_binding *binding;
    /* Original xxxSetWindowData resolves CPD handles before storing lpfnWndProc.
     * This storage-only API cannot accept a CPD as an encoded WOW procedure. */
    if (!previous || !procedure || HIWORD(procedure) == WNDPROC_HANDLE ||
            procedure == (DWORD)wow_window_native_proc) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    binding = wow_window_words_acquire(window);
    if (!binding) return FALSE;
    if ((procedure & WNDPROC_WOW) && !binding->callback) {
        wow_window_words_release(binding);
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    *previous = (DWORD)InterlockedExchange((LONG *)&binding->cleanup.lpfnWndProc, (LONG)procedure);
    wow_window_words_release(binding);
    return TRUE;
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
