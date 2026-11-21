#include "wow_idle_hook_binding.h"
#include "wow_startup_report.h"
#include "compat/include/wow_user_runtime.h"
#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"

/* This native observer has left its guest callback scope before forwarding.
 * The next native hook owns its own callback entry; do not project guest
 * state from the unowned observer continuation. */
#undef CallNextHookEx

static const wow_task_state_callbacks *callbacks;

BOOL WINAPI wow_task_state_bind(const wow_task_state_callbacks *provider)
{
    const wow_task_state_callbacks *previous;
    if (!provider || !provider->idle_registered || !provider->separate_launch ||
            !provider->idle) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&callbacks,
        (PVOID)provider, NULL);
    if (previous && previous != provider) {
        SetLastError(ERROR_ALREADY_INITIALIZED);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI wow_user_idle_hook_registered(DWORD native_thread_id)
{
    return callbacks->idle_registered(native_thread_id);
}

BOOL WINAPI wow_current_separate_launch(void)
{
    return callbacks->separate_launch();
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
        result = callbacks->idle(code, wp, lp);
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
