#include "wow_timer_callback.h"
#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"

/* Retained wutmr.c owns slot lookup, queued callbacks after cancellation,
 * argument conversion and CallBack16. Protect its whole body whether native
 * USER dispatches a registered timer or the original WM_TIMER converter
 * supplies the same original stub. No timer identity is replaced here. */
static wow_timer_callback callback;
BOOL WINAPI wow_timer_bind(wow_timer_callback value)
{
    wow_timer_callback previous;
    if (!value) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    previous = (wow_timer_callback)InterlockedCompareExchangePointer(
        (PVOID volatile *)&callback, (PVOID)value, NULL);
    if (previous && previous != value) {
        SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE;
    }
    return TRUE;
}

VOID W32TimerFunc(UINT index, HWND window, UINT event, DWORD time)
{
    wow_task_callback_scope task;
    wow_user_borrow_scope borrow;
    BOOL borrowed = FALSE;
    if (!wow_task_callback_enter(&task)) return;
    __try {
        if (!wow_user_borrow_enter(&borrow)) __leave;
        borrowed = TRUE;
        if (!callback) RaiseFailFastException(NULL, NULL, 0);
        callback(index, window, event, time);
    } __finally {
        __try {
            if (borrowed) wow_user_borrow_leave_required(&borrow);
        } __finally {
            (void)wow_task_callback_leave(&task);
        }
    }
}
