#include "wow_dialog_creation_binding.h"
#include "wow_dialog_context_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_borrow_scope.h"

BOOL WINAPI wow_original_IsDialogMessageA(HWND, LPMSG);
BOOL WINAPI wow_original_IsDialogMessageW(HWND, LPMSG);

/* OPENNT-HOST-119: original manager owns all keyboard behavior. This is
 * the common modeless/modal execution and retained-view boundary only. */
static BOOL dialog_message(HWND window, LPMSG message, BOOL ansi)
{
    DWORD error = GetLastError();
    BOOL result = FALSE;
    wow_task_callback_scope task;
    wow_user_borrow_scope borrow;
    wow_dialog_context context = {0}, *previous = wow_dialog_current;
    if (!message) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!wow_task_callback_enter(&task)) return FALSE;
    if (!wow_user_borrow_enter(&borrow)) {
        error = GetLastError();
        (void)wow_task_callback_leave(&task);
        SetLastError(error);
        return FALSE;
    }
    __try {
        if (!wow_dialog_capture_context(&context, FALSE)) __leave;
        wow_dialog_current = &context;
        SetLastError(error);
        result = ansi ? wow_original_IsDialogMessageA(window, message) :
            wow_original_IsDialogMessageW(window, message);
    } __finally {
        error = GetLastError();
        wow_dialog_current = previous;
        wow_dialog_release_context(&context);
        wow_user_borrow_leave_required(&borrow);
        (void)wow_task_callback_leave(&task);
        SetLastError(error);
    }
    return result;
}

BOOL WINAPI wow_dialog_messageA(HWND window, LPMSG message)
{
    return dialog_message(window, message, TRUE);
}

BOOL WINAPI wow_dialog_messageW(HWND window, LPMSG message)
{
    return dialog_message(window, message, FALSE);
}
