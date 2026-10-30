#include "precomp.h"
#include "wow_idle_hook_binding.h"
#include "wow_user_runtime.h"
#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"

/* This native observer has left its guest callback scope before forwarding.
 * The next native hook owns its own callback entry; do not project guest
 * state from the unowned observer continuation. */
#undef CallNextHookEx

extern PTD gptdTaskHead;
extern CRITICAL_SECTION gcsWOW;
LRESULT CALLBACK WowOriginalForegroundIdleHook(int, WPARAM, LPARAM);

BOOL WINAPI wow_user_idle_hook_registered(DWORD native_thread_id)
{
    PTD task;
    BOOL registered = FALSE;
    DWORD error = GetLastError();
    /* Before original WOW initialization there is no TD list or initialized
     * list lock. No registration can exist in that state. Recheck under the
     * lock before ever dereferencing a nonempty borrowed head. */
    if (!gptdTaskHead) return FALSE;
    /* wkman.c owns both the list and hIdleHook. Hold its existing lock only
     * while borrowing list nodes, never across native or guest callbacks. */
    EnterCriticalSection(&gcsWOW);
    for (task = gptdTaskHead; task; task = task->ptdNext) {
        /* W32Thread links its TD before initializing hIdleHook. hThread is
         * NULL before linkage and published only after successful InitTask. */
        /* The original INT 2F:1689 vector is worker-shared: registration by
         * one live WOW task enables taskman's idle notification while any
         * task in that worker is scheduled.  A nonzero thread asks for the
         * observer owned by that exact task; zero asks only whether the
         * original shared intent exists.  The callback itself still runs in
         * the currently scheduled task's scope. */
        if ((!native_thread_id || task->dwThreadID == native_thread_id) &&
                task->hThread && task->hIdleHook) {
            registered = TRUE;
            break;
        }
    }
    LeaveCriticalSection(&gcsWOW);
    SetLastError(error);
    return registered;
}

static LRESULT dispatch_idle(int code, WPARAM wp, LPARAM lp)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_task_callback_scope task;
    wow_user_borrow_scope borrow;
    BOOL borrowed = FALSE;
    LRESULT result = 0;
    /* Thread scope excludes other processes; any live TD registration in
     * this worker gates the current task's observer. Never treat an unbound
     * TEB as a TD. */
    if (code < 0 || !binding || !binding->thread || !binding->thread->ptdb ||
            !wow_user_idle_hook_registered(0))
        return 0;
    if (!wow_task_callback_enter(&task)) return 0;
    __try {
        if (!wow_user_borrow_enter(&borrow)) __leave;
        borrowed = TRUE;
        result = WowOriginalForegroundIdleHook(code, wp, lp);
    } __finally {
        __try {
            if (borrowed) wow_user_borrow_leave_required(&borrow);
        } __finally {
            (void)wow_task_callback_leave(&task);
        }
    }
    return result;
}

LRESULT CALLBACK WK32ForegroundIdleHook(int code, WPARAM wp, LPARAM lp)
{
    LRESULT result;
    DWORD error;
    (void)dispatch_idle(code, wp, lp);
    error = GetLastError();
    /* Owner accepts thread-before-global ordering, not swallowing native
     * observers. Taskman's direct entry must not call into this native chain. */
    result = CallNextHookEx(NULL, code, wp, lp);
    SetLastError(error);
    return result;
}

void WINAPI wow_user_idle_hook_dispatch(void)
{
    /* Original taskman selects the current task, calls once, clears its
     * selection and rescans. Retain that algorithm and original RET callback. */
    (void)dispatch_idle(HC_ACTION, 0, 0);
}
