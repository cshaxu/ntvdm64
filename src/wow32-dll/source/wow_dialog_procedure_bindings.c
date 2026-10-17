#include "wow_dialog_procedure_bindings.h"
#include "wow_user_task_lifecycle.h"

LRESULT WINAPI wow_user_call_window_procA(WNDPROC, HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_user_call_window_procW(WNDPROC, HWND, UINT, WPARAM, LPARAM);
C_ASSERT(sizeof(LONG_PTR) == sizeof(LONG));

LONG WINAPI wow_dialog_host_get(HWND w, int index, BOOL ansi)
{
    return ansi ? GetWindowLongA(w, index) : GetWindowLongW(w, index);
}

LONG WINAPI wow_dialog_host_set(HWND w, int index, LONG value, BOOL ansi)
{
    return ansi ? SetWindowLongA(w, index, value) : SetWindowLongW(w, index, value);
}

static LRESULT native_default(HWND w, UINT m, WPARAM a, LPARAM b, BOOL ansi)
{
    DWORD error = GetLastError();
    LRESULT result = 0;
    wow_user_native_call call;
    if (!wow_user_native_call_begin(&call, w))
        wow_user_native_call_failed("dialog-procedure-enter", GetLastError());
    __try {
        SetLastError(error);
        result = ansi ? DefDlgProcA(w, m, a, b) : DefDlgProcW(w, m, a, b);
        wow_user_native_call_result(&call, m, a, b, result);
    } __finally {
        error = GetLastError();
        if (!wow_user_native_call_end(&call))
            wow_user_native_call_failed("dialog-procedure-return", GetLastError());
        SetLastError(error);
    }
    return result;
}

BOOL WINAPI wow_dialog_host_initialize(HWND w)
{
    DWORD error = GetLastError(), failure;
    LONG saved;
    /* First recovered promotion only. Save/restore through W, including
     * opaque tokens. Null the real slot during native initialization so
     * WM_NULL cannot become an extra application callback. No helper HWND. */
    SetLastError(0);
    saved = SetWindowLongW(w, DWL_DLGPROC, 0);
    failure = GetLastError();
    if (!saved && failure) return FALSE;
    __try {
        (void)native_default(w, WM_NULL, 0, 0, FALSE);
    } __finally {
        DWORD unwind_error = GetLastError();
        BOOL unwinding = AbnormalTermination();
        SetLastError(0);
        (void)SetWindowLongW(w, DWL_DLGPROC, saved);
        failure = GetLastError();
        if (failure)
            wow_user_native_call_failed("dialog-initialize-restore", failure);
        SetLastError(unwinding ? unwind_error : error);
    }
    return failure == 0;
}

LRESULT WINAPI wow_dialog_host_call(HWND w, UINT m, WPARAM a, LPARAM b, BOOL ansi)
{
    WNDPROC proc = (WNDPROC)(LONG_PTR)wow_dialog_host_get(w, DWL_DLGPROC, ansi);
    if (!proc) return 0;
    return ansi ? wow_user_call_window_procA(proc, w, m, a, b) :
        wow_user_call_window_procW(proc, w, m, a, b);
}

LRESULT WINAPI wow_dialog_host_notify(HWND w, UINT m, WPARAM a, LPARAM b, BOOL ansi)
{
    /* Invoked instead of, not after, the original application dispatch. */
    return native_default(w, m, a, b, ansi);
}

VOID WINAPI wow_dialog_host_retire(HWND w)
{
    DWORD error = GetLastError();
    SetLastError(0);
    if (!SetWindowLongW(w, DWL_DLGPROC, 0) && GetLastError())
        wow_user_native_call_failed("dialog-procedure-retire", GetLastError());
    (void)native_default(w, WM_NCDESTROY, 0, 0, FALSE);
    SetLastError(error);
}

LRESULT CALLBACK wow_dialog_host_terminal_proc(HWND w, UINT m, WPARAM a, LPARAM b)
{
    if (m == WM_NCDESTROY) {
        SetLastError(0);
        if (!SetWindowLongW(w, DWL_DLGPROC, 0) && GetLastError())
            wow_user_native_call_failed("dialog-exit-procedure-retire", GetLastError());
        return DefDlgProcW(w, m, a, b);
    }
    return DefWindowProcW(w, m, a, b);
}
