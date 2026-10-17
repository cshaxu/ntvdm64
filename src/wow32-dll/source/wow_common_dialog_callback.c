#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_object_bindings.h"
#include "wow_window_words_binding.h"

typedef UINT (WINAPI *common_dialog_callback)(HWND, UINT, WPARAM, LPARAM);
UINT WINAPI WowOriginalDialogProc(HWND, UINT, WPARAM, LPARAM);
UINT WINAPI WowOriginalPrintSetupDialogProc(HWND, UINT, WPARAM, LPARAM);
UINT WINAPI WowOriginalFindReplaceDialogProc(HWND, UINT, WPARAM, LPARAM);

static UINT invoke_dialog(common_dialog_callback original, HWND window,
    UINT message, WPARAM wp, LPARAM lp, BOOL resource_cleanup,
    wow_window_resource_destroy resource_owner)
{
    wow_task_callback_scope task;
    wow_user_borrow_scope borrow;
    BOOL borrowed = FALSE;
    UINT result = FALSE;
    if (!(resource_cleanup ? wow_task_resource_cleanup_enter(&task) :
            wow_task_callback_enter(&task))) {
        if (resource_cleanup)
            wow_user_native_call_failed("enter-dialog-resource-cleanup", GetLastError());
        return FALSE;
    }
    __try {
        if (!wow_user_borrow_enter(&borrow)) {
            if (resource_cleanup)
                wow_user_native_call_failed("borrow-dialog-resource-cleanup", GetLastError());
            __leave;
        }
        borrowed = TRUE;
        if (!wow_user_window_refresh_message(window, message, wp, lp)) {
            if (resource_cleanup)
                wow_user_native_call_failed("publish-dialog-resource-cleanup", GetLastError());
            __leave;
        }
        /* A persistent original native resource owner must still receive
         * its resource-only destroy after dying application delivery is
         * neutralized. Bind before the first app hook can destroy/reenter. */
        if(resource_owner && message==WM_INITDIALOG &&
                !wow_window_words_bind_resource_destroy(window,resource_owner))
            wow_user_native_call_failed("bind-dialog-resource-owner",GetLastError());
        if(resource_owner && message==WM_DESTROY &&
                !wow_window_words_consume_resource_destroy(window,resource_owner,FALSE))
            wow_user_native_call_failed("consume-dialog-resource-owner",GetLastError());
        result = original(window, message, wp, lp);
    } __finally {
        __try {
            if (borrowed) wow_user_borrow_leave_required(&borrow);
        } __finally {
            (void)wow_task_callback_leave(&task);
        }
    }
    return result;
}

UINT WINAPI WCD32DialogProc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    return invoke_dialog(WowOriginalDialogProc, window, message, wp, lp, FALSE,NULL);
}

UINT WINAPI WCD32PrintSetupDialogProc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    return invoke_dialog(WowOriginalPrintSetupDialogProc, window, message, wp, lp, FALSE,NULL);
}

UINT WINAPI WCD32FindReplaceDialogProc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    /* Original WM_DESTROY does not invoke the app hook. It owns resource and
     * host-context release and must run before guest GlobalFreeAll, not
     * merely before the later host W32FreeTask resource-record teardown. */
    return invoke_dialog(WowOriginalFindReplaceDialogProc, window, message,
        wp, lp, message == WM_DESTROY,WCD32FindReplaceDialogProc);
}
