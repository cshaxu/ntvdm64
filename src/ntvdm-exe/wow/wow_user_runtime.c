#include "wow_user_runtime.h"

static __declspec(thread) wow_user_runtime_thread *current_thread;

static BOOL CALLBACK initialize_runtime(PINIT_ONCE once, PVOID parameter,
    PVOID *context)
{
    wow_user_runtime *runtime = parameter;
    UNREFERENCED_PARAMETER(once);
    UNREFERENCED_PARAMETER(context);
    InitializeSRWLock(&runtime->exclusive);
    runtime->process_id = GetCurrentProcessId();
    runtime->owner_thread_id = 0;
    return TRUE;
}

BOOL WINAPI wow_user_runtime_initialize(wow_user_runtime *runtime)
{
    if (!runtime) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    return InitOnceExecuteOnce(&runtime->initialization, initialize_runtime,
        runtime, NULL);
}

BOOL WINAPI wow_user_runtime_bind(wow_user_runtime_thread *binding,
    wow_user_runtime *runtime, struct wow_task_order_thread *thread,
    struct wow_class_client_context *classes)
{
    if (!binding || !runtime || current_thread || binding->runtime ||
            binding->exclusive_held || !wow_user_runtime_initialize(runtime)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    binding->runtime = runtime;
    binding->thread = thread;
    binding->classes = classes;
    current_thread = binding;
    return TRUE;
}

BOOL WINAPI wow_user_runtime_unbind(wow_user_runtime_thread *binding)
{
    if (!binding || current_thread != binding || binding->exclusive_held) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* USER resource cleanup may precede the end of guest execution. Retire
     * the provider task only at this existing native-thread unbind edge. */
    if (binding->thread && binding->runtime->retire_thread &&
            !binding->runtime->retire_thread(binding->runtime->lifecycle))
        return FALSE;
    current_thread = NULL;
    binding->runtime = NULL;
    binding->thread = NULL;
    binding->classes = NULL;
    return TRUE;
}

wow_user_runtime_thread *WINAPI wow_user_runtime_current(void)
{
    return current_thread;
}

BOOL WINAPI wow_user_runtime_enter(wow_user_runtime_thread *binding)
{
    if (!binding || current_thread != binding || !binding->runtime ||
            binding->exclusive_held) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    AcquireSRWLockExclusive(&binding->runtime->exclusive);
    binding->runtime->owner_thread_id = GetCurrentThreadId();
    binding->exclusive_held = TRUE;
    return TRUE;
}

BOOL WINAPI wow_user_runtime_leave(wow_user_runtime_thread *binding)
{
    if (!binding || current_thread != binding || !binding->runtime ||
            !binding->exclusive_held ||
            binding->runtime->owner_thread_id != GetCurrentThreadId()) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    binding->exclusive_held = FALSE;
    binding->runtime->owner_thread_id = 0;
    ReleaseSRWLockExclusive(&binding->runtime->exclusive);
    return TRUE;
}

BOOL WINAPI wow_user_runtime_leave_for_callout(wow_user_runtime_thread *binding)
{
    return wow_user_runtime_leave(binding);
}

BOOL WINAPI wow_user_runtime_reenter_after_callout(wow_user_runtime_thread *binding)
{
    return wow_user_runtime_enter(binding);
}

BOOL WINAPI wow_user_runtime_set_context(wow_user_runtime_thread *binding,
    struct wow_task_order_thread *thread,
    struct wow_class_client_context *classes)
{
    if (!binding || current_thread != binding || !binding->runtime) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    binding->thread = thread;
    binding->classes = classes;
    return TRUE;
}
