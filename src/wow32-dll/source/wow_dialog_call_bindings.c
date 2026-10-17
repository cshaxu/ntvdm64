#include "wow_dialog_native_bindings.h"
#include "wow_dialog_state_bindings.h"
#include "wow_user_task_lifecycle.h"

VOID WINAPI wow_dialog_set_pointer_word(HWND window, UINT pointer)
{
    if (!wow_dialog_set_pointer(window, (PDLG)(ULONG_PTR)pointer))
        wow_user_native_call_failed("dialog-pointer-state", GetLastError());
}

BOOL WINAPI wow_dialog_set_system_menu(HWND window)
{
    DWORD error = GetLastError();
    BOOL result = FALSE;
    wow_user_native_call call;
    if (!wow_dialog_native_ready())
        wow_user_native_call_failed("dialog-menu-service", GetLastError());
    if (!wow_user_native_call_begin(&call, window))
        wow_user_native_call_failed("dialog-menu-enter", GetLastError());
    __try {
        SetLastError(error);
        result = wow_dialog_native_set_system_menu(window);
    } __finally {
        error = GetLastError();
        if (!wow_user_native_call_end(&call))
            wow_user_native_call_failed("dialog-menu-return", GetLastError());
        SetLastError(error);
    }
    return result;
}

VOID WINAPI wow_dialog_set_thread_state(DWORD flags, DWORD mask)
{
    /* Original dialog activation/cleanup owns the queue flag lifetime.
     * This native operation has no application callout or HWND mutation. */
    if (!wow_dialog_native_set_thread_state(flags, mask))
        wow_user_native_call_failed("dialog-thread-state", GetLastError());
}

/* ADAPTER-WOW-062: the original EndDialog algorithm owns when this operation
 * occurs. Only the current native queue reset crosses this call boundary;
 * no replacement activation, focus, notification or task policy lives here. */
VOID WINAPI wow_dialog_zap_active_and_focus(void)
{
    DWORD error = GetLastError();
    wow_user_native_call call;
    BOOL invoked = FALSE;
    if (!wow_user_native_call_begin(&call, NULL))
        wow_user_native_call_failed("dialog-reset-enter", GetLastError());
    __try {
        SetLastError(error);
        invoked = wow_dialog_native_zap_active_and_focus();
    } __finally {
        error = GetLastError();
        if (!wow_user_native_call_end(&call))
            wow_user_native_call_failed("dialog-reset-return", GetLastError());
        SetLastError(error);
    }
    if (!invoked)
        wow_user_native_call_failed("dialog-reset-service", error);
}
