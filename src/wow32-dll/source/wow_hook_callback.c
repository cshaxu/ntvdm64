#include "precomp.h"
#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_object_bindings.h"
#include "wow_window_creation_binding.h"
#include "wow_user_runtime.h"

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
    BOOL creation = FALSE, completed = FALSE, task_left = FALSE;
    HOOKSTATEDATA hook = {0};
    wow_window_creation_enrollment enrollment = {0};
    wow_user_runtime_thread *runtime;
    if (!wow_task_callback_enter(&task)) return 0;
    __try {
        /* Read identity through the original table owner, not the numerical
         * hook code alone (codes overlap between different hook types).
         * A newer guest hook can run before our native creation observer. */
        if (index > 0 && index <= 255) {
            hook.iIndex = (BYTE)index;
            creation = W32GetHookStateData(&hook) && hook.InUse &&
                hook.iHook == WH_CBT && code == HCBT_CREATEWND && lp;
        }
        if (creation) result = 1; /* Incomplete setup vetoes creation. */
        if (!wow_user_borrow_enter(&borrow)) __leave;
        borrowed = TRUE;
        runtime = wow_user_runtime_current();
        if (creation && !wow_window_creation_enroll_internal(
                runtime ? runtime->thread : NULL, (HWND)wp,
                ((CBT_CREATEWNDA *)lp)->lpcs, &enrollment)) __leave;
        if (!wow_user_window_refresh_graph(creation ? (HWND)wp : NULL)) __leave;
        result = WowOriginalStdHookProc(code, wp, lp, index);
        completed = TRUE;
    } __finally {
        __try {
            if (creation) {
                BOOL accepted = completed && result == 0;
                /* Keep the exact enrollment borrowed until the final veto
                 * decision. Never reacquire a possibly reused HWND later.
                 * No guest alias escapes this synchronous hook call. */
                completed = FALSE;
                task_left = TRUE;
                if (!wow_task_callback_leave(&task)) result = 1;
                else completed = accepted;
            }
        } __finally {
            wow_window_creation_finish(&enrollment, completed && result == 0);
            /* Final window/class references still retire before alias
             * collection, including a failed task-scope return. */
            if (borrowed) (void)wow_user_borrow_leave(&borrow);
            if (!task_left && !wow_task_callback_leave(&task)) result = 0;
        }
    }
    return result;
}
