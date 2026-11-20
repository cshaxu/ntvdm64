#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"

/* Retained wutmr.c owns slot lookup, queued callbacks after cancellation,
 * argument conversion and CallBack16. Protect its whole body whether native
 * USER dispatches a registered timer or the original WM_TIMER converter
 * supplies the same original stub. No timer identity is replaced here. */
VOID WowOriginalTimerFunc(UINT, HWND, UINT, DWORD);

VOID W32TimerFunc(UINT index, HWND window, UINT event, DWORD time)
{
    wow_task_callback_scope task;
    wow_user_borrow_scope borrow;
    BOOL borrowed = FALSE;
    if (!wow_task_callback_enter(&task)) return;
    __try {
        if (!wow_user_borrow_enter(&borrow)) __leave;
        borrowed = TRUE;
        WowOriginalTimerFunc(index, window, event, time);
    } __finally {
        __try {
            if (borrowed) wow_user_borrow_leave_required(&borrow);
        } __finally {
            (void)wow_task_callback_leave(&task);
        }
    }
}
