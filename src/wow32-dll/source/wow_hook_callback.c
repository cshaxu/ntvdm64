#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_object_bindings.h"

/* Original wowhooks.c owns hook identity, conversion, guest-stack allocation
 * and dispatch. Protect that whole interval, not just its final CallBack16.
 * All 32 unchanged native hook stubs still call WU32StdHookProc. */
LONG WINAPI WowOriginalStdHookProc(INT, LONG, LONG, INT);

LONG WINAPI WU32StdHookProc(INT code, LONG wp, LONG lp, INT index)
{
    wow_task_callback_scope task;
    wow_user_borrow_scope borrow;
    LONG result = 0;
    BOOL borrowed = FALSE;
    if (!wow_task_callback_enter(&task)) return 0;
    __try {
        if (!wow_user_borrow_enter(&borrow)) __leave;
        borrowed = TRUE;
        if (!wow_user_window_refresh_graph(NULL)) __leave;
        result = WowOriginalStdHookProc(code, wp, lp, index);
    } __finally {
        if (borrowed) (void)wow_user_borrow_leave(&borrow);
        if (!wow_task_callback_leave(&task)) result = 0;
    }
    return result;
}
