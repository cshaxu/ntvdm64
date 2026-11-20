#include "non-mvdm/public/sdk/inc/nt.h"
#include "include/wow_user_registration_state.h"
#include "include/wow_class_words_binding.h"
#include "include/wow_window_words_binding.h"
#include "include/wow_user_object_bindings.h"
#include "include/wow_user_borrow_scope.h"
#include "include/wow_menu_native_bindings.h"
#include "include/wow_edit_call_bindings.h"
#include <stdio.h>

static PFNWOWHANDLERSIN wow_input_handlers;
static wow_user_task_lifecycle wow_lifecycle;

void wow_user_registration_trace(const char *stage, DWORD result)
{
    char path[MAX_PATH];
    char line[96];
    HANDLE file;
    DWORD bytes;
    DWORD written;

    if (!GetEnvironmentVariableA("MVDM_WOW_REGISTRATION_TRACE_PATH", path,
            sizeof(path)))
        return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return;
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu %s result=%08lX\r\n", (unsigned long)GetCurrentProcessId(),
        stage, (unsigned long)result);
    if (bytes)
        (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}


static void registration_trace_class(LPCSTR name, ATOM result)
{
    char path[MAX_PATH];
    char line[192];
    HANDLE file;
    DWORD bytes;
    DWORD written;

    if (!GetEnvironmentVariableA("MVDM_WOW_REGISTRATION_TRACE_PATH", path,
            sizeof(path))) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu RegisterClassName name=%s atom=%04X\r\n",
        (unsigned long)GetCurrentProcessId(), name ? name : "<null>",
        (unsigned)result);
    if (bytes) (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}

static void registration_trace_context(const wow_class_lookup_context *classes)
{
    char path[MAX_PATH];
    char line[96];
    HANDLE file;
    DWORD bytes;
    DWORD written;
    if (!GetEnvironmentVariableA("MVDM_WOW_REGISTRATION_TRACE_PATH", path,
            sizeof(path))) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu RegisterClassContext value=%08lX private=%08lX public=%08lX\r\n",
        (unsigned long)GetCurrentProcessId(), (unsigned long)(ULONG_PTR)classes,
        (unsigned long)(ULONG_PTR)(classes ? classes->pclsPrivateList : NULL),
        (unsigned long)(ULONG_PTR)(classes ? classes->pclsPublicList : NULL));
    if (bytes) (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}

static void unsupported_void(void)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}

BOOL wow_user_ensure_registration_lifecycle(void)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_user_task_lifecycle_callbacks callbacks;

    if (!binding || !binding->runtime) {
        unsupported_void();
        return FALSE;
    }
    if (wow_user_registered_lifecycle()->initialized)
        return wow_user_registered_lifecycle()->runtime == binding->runtime;
    callbacks.get_version = wow_user_registered_callbacks()->pfnGetExpWinVer;
    callbacks.callback = wow_user_registered_callbacks()->pfnWowWndProcEx;
    return wow_user_task_lifecycle_initialize(wow_user_registered_lifecycle(), binding->runtime,
        &callbacks);
}


HWND WINAPI registered_create_window_ex(DWORD ex_style,
    LPCTSTR class_name, LPCTSTR window_name, DWORD style, int x, int y,
    int width, int height, HWND parent, HMENU menu, HANDLE instance,
    LPVOID param, DWORD flags, LPDWORD wow_words)
{
    HWND result = wow_user_create_window_ex(ex_style, class_name, window_name,
        style, x, y, width, height, parent, menu, instance, param, flags,
        wow_words);
    DWORD error=GetLastError();
    wow_user_registration_trace("CreateWindowEx", (DWORD)(ULONG_PTR)result);
    SetLastError(error);
    return result;
}

ATOM WINAPI registered_register_class(PVOID a, LPDWORD b)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!binding || !binding->classes) {
        unsupported_void(); return 0;
    }
    {
        ATOM result;
        registration_trace_context(binding->classes->classes);
        result = wow_class_client_register_guest(binding->classes, (WNDCLASSA *)a, b);
        registration_trace_context(binding->classes->classes);
        wow_user_registration_trace("RegisterClass", result);
        registration_trace_class(((WNDCLASSA *)a)->lpszClassName, result);
        return result;
    }
}

BOOL WINAPI registered_hung_handlers(PFNW32ET callback, HANDLE event)
{
    BOOL result = wow_user_ensure_registration_lifecycle() && wow_user_task_lifecycle_register(wow_user_registered_lifecycle(),
        callback, event);
    wow_user_registration_trace("RegisterHung", result);
    return result;
}

BOOL WINAPI registered_cleanup(HANDLE instance, DWORD task,
    PNEMODULESEG selectors, DWORD count)
{
    BOOL result = FALSE;
    if (wow_user_ensure_registration_lifecycle()) {
        /* Original W32DestroyTask also calls here after InitTask rollback.
         * No USER task was admitted, so there are no task-owned USER objects
         * to destroy. Module unload and committed-task failures still require
         * the full original cleanup; absence alone is never success. */
        if (task && !selectors && !count &&
                wow_user_task_cleanup_uninitialized(task))
            result = TRUE;
        else
            result = wow_user_task_lifecycle_exit(wow_user_registered_lifecycle(),
                instance, task, selectors, count);
    }
    DWORD error = result ? ERROR_SUCCESS : GetLastError();
    wow_user_registration_trace("WOWCleanup", result);
    /* Original ModuleUnload does not consume this BOOL. Never let guest
     * GlobalFreeAll invalidate resources after rejected task cleanup. */
    if (!result) wow_user_native_call_failed("registered-task-cleanup", error);
    return result;
}

DWORD WINAPI registered_full_handle(WORD handle)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();

    if (!binding || !binding->thread || !binding->thread->ppi ||
            !binding->thread->ppi->objects) {
        unsupported_void();
        return 0;
    }
    /* This is the original all-WOW conversion callback.  Its value is the
     * NT4 shared USER index/uniqueness token, not a public native HWND.
     * A public-USER caller must use an explicitly scoped adapter rather than
     * changing every original FULLHWND32 consumer (messages, DDE, dialogs)
     * to a different representation. */
    {
        DWORD value = wow_user_full_handle(binding->thread->ppi->objects, handle);
        wow_user_registration_trace("GetFullUserHandle", value);
        return value;
    }
}

VOID WINAPI registered_directed_yield(DWORD thread_id)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!binding || !binding->thread) { unsupported_void(); return; }
    wow_user_task_lifecycle_directed_yield(wow_user_registered_lifecycle(), thread_id);
    wow_user_registration_trace("DirectedYield", thread_id);
}

BOOL WINAPI registered_yield_task(void)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    BOOL result;
    if (!binding || !binding->thread) { unsupported_void(); return FALSE; }
    result = wow_user_task_lifecycle_yield(wow_user_registered_lifecycle());
    wow_user_registration_trace("YieldTask", result);
    return result;
}

BOOL WINAPI registered_wait_for_message(HANDLE event)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    BOOL result;
    if (!binding || !binding->thread) { unsupported_void(); return FALSE; }
    result = wow_user_task_lifecycle_wait(wow_user_registered_lifecycle(), event);
    wow_user_registration_trace("WaitForMessage", result);
    return result;
}

VOID WINAPI registered_fill_window(HWND parent, HWND window, HDC dc,
    HANDLE brush)
{
    (void)wow_edit_FillWindow(parent, window, dc, (HBRUSH)brush);
}


VOID WINAPI wow_user_register_wow_exec(HANDLE window)
{
    VOID WINAPI RegisterWowExec(HANDLE);
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    DWORD full;
    if (!binding || !binding->thread || !binding->thread->ppi ||
            !binding->thread->ppi->objects) {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return;
    }
    /* Registration requires a live owned window, unlike original message
     * conversion's intentional fallback for a departed DDE peer. */
    full = (DWORD)(ULONG_PTR)wow_user_window_full_handle(
        binding->thread->ppi->objects, LOWORD(window));
    /* WK32RegisterShellWindowHandle supplies HWND32, a sign-extended WORD.
     * Resolve it through the existing USER handle owner before the original
     * BaseClient sends it to BaseSrv. Never weaken broker PID validation. */
    if (!full) return;
    if ((DWORD)(ULONG_PTR)window != (DWORD)(LONG)(SHORT)LOWORD(window) &&
            (DWORD)(ULONG_PTR)window != full) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return;
    }
    RegisterWowExec((HANDLE)(ULONG_PTR)full);
}

const struct tagAPFNWOWHANDLERSIN *WINAPI wow_user_registered_callbacks(void)
{
    return &wow_input_handlers;
}


BOOL WINAPI wow_user_get_messageA(LPMSG message, HWND window, UINT first, UINT last)
{
    return wow_user_task_lifecycle_message(&wow_lifecycle, message, window,
        first, last, PM_REMOVE, TRUE);
}

BOOL WINAPI wow_user_peek_messageA(LPMSG message, HWND window, UINT first,
    UINT last, UINT flags)
{
    return wow_user_task_lifecycle_message(&wow_lifecycle, message, window,
        first, last, flags, FALSE);
}

BOOL WINAPI wow_user_wait_message(void)
{
    return wow_user_task_lifecycle_wait_message(&wow_lifecycle);
}

BOOL WINAPI wow_user_get_messageW(LPMSG message, HWND window, UINT first, UINT last)
{
    return wow_user_task_lifecycle_messageW(&wow_lifecycle, message, window,
        first, last, PM_REMOVE, TRUE);
}

BOOL WINAPI wow_user_peek_messageW(LPMSG message, HWND window, UINT first,
    UINT last, UINT flags)
{
    return wow_user_task_lifecycle_messageW(&wow_lifecycle, message, window,
        first, last, flags, FALSE);
}

static LRESULT call_previous_window_proc(WNDPROC procedure, HWND window,
    UINT message, WPARAM wp, LPARAM lp, BOOL unicode)
{
    wow_user_borrow_scope scope;
    wow_task_callback_scope task_scope;
    LRESULT result;
    DWORD resolved;
    /* Original clmsg.c::CallWindowProcAorW resolves CPD first, then
     * usercli.h::CALLPROC_WOWCHECK forwards tagged WOW targets with NULL WW.
     * Modern USER owns native/CPD translation but cannot interpret this
     * worker's WOW target. Keep the original registered callback as owner. */
    if (!wow_native_class_resolve_procedure((DWORD)procedure, &resolved)) return 0;
    if (resolved != (DWORD)procedure ||
            !((DWORD)(ULONG_PTR)procedure & WNDPROC_WOW)) {
        wow_user_native_call call;
        BOOL restored;
        DWORD error = GetLastError();
        wow_window_note_native_procedure(window, procedure, unicode);
        if (!wow_user_native_call_begin(&call, window)) return 0;
        SetLastError(error);
        __try {
            result = unicode ? CallWindowProcW(procedure, window, message, wp, lp) :
                CallWindowProcA(procedure, window, message, wp, lp);
            wow_user_native_call_result(&call, message, wp, lp, result);
        } __finally {
            error = GetLastError();
            restored = wow_user_native_call_end(&call);
            if (restored) SetLastError(error);
        }
        return restored ? result : 0;
    }
    if (!wow_input_handlers.pfnWowWndProcEx) {
        SetLastError(ERROR_INVALID_STATE);
        return 0;
    }
    if (!wow_task_callback_enter(&task_scope)) return 0;
    if (!wow_user_borrow_enter(&scope)) {
        (void)wow_task_callback_leave(&task_scope);
        return 0;
    }
    __try {
        /* CallWindowProc's tagged-WOW branch deliberately passes NULL WW;
         * it does not traverse wow_window_dispatch_bound. Preserve that
         * original subclass contract while publishing a menu carried by
         * the message before the original callback converts its handle. */
        result = 0;
        if (wow_user_window_refresh_message(window, message, wp, lp) &&
                wow_menu_native_refresh_message(message, wp, lp))
            result = wow_input_handlers.pfnWowWndProcEx(window, message, wp, lp,
                (DWORD)(ULONG_PTR)procedure, NULL);
    } __finally {
        wow_user_borrow_leave_required(&scope);
        (void)wow_task_callback_leave(&task_scope);
    }
    return result;
}

LRESULT WINAPI wow_user_call_window_procA(WNDPROC procedure, HWND window,
    UINT message, WPARAM wp, LPARAM lp)
{
    return call_previous_window_proc(procedure, window, message, wp, lp, FALSE);
}

LRESULT WINAPI wow_user_call_window_procW(WNDPROC procedure, HWND window,
    UINT message, WPARAM wp, LPARAM lp)
{
    return call_previous_window_proc(procedure, window, message, wp, lp, TRUE);
}

wow_user_task_lifecycle *WINAPI wow_user_registered_lifecycle(void)
{
    return &wow_lifecycle;
}

void WINAPI wow_user_set_registered_callbacks(APFNWOWHANDLERSIN input)
{
    wow_input_handlers = *input;
}
