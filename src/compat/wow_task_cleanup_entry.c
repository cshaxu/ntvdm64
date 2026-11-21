#include "wow_task_cleanup_entry.h"
#include "compat/include/wow_user_runtime.h"
#include "wow_task_order_bindings.h"

static const wow_task_cleanup_provider *provider;
BOOL WINAPI wow_task_cleanup_bind(const wow_task_cleanup_provider *value)
{
    const wow_task_cleanup_provider *previous;
    if (!value || !value->uninitialized || !value->retire_async || !value->free_task) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&provider,
        (PVOID)value, NULL);
    if (previous && previous != value) {
        SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE;
    }
    return TRUE;
}
static const wow_task_cleanup_provider *current_provider(void)
{
    if (!provider) RaiseFailFastException(NULL, NULL, 0);
    return provider;
}
BOOL WINAPI wow_user_task_cleanup_uninitialized(DWORD task_id)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    /* The original owner verifies the still-linked, unadmitted current TD. */
    if (!task_id || task_id > 0xffff || !binding || !binding->runtime ||
            binding->thread || binding->classes) return FALSE;
    return current_provider()->uninitialized(task_id);
}

/* The cleanup call retains its opaque pointer ABI and original /Gz convention;
 * wkman.c retains the complete task/resource cleanup implementation.
 * Normal task windows are already retired by SG_EXIT's pfnWOWCleanup,
 * before guest GlobalFreeAll. This is the fallback for original remote-task
 * and incomplete-startup exits which do not necessarily send that signal. */

/* Retire async guest-buffer users before the guest frees task segments.
 * The original late cleanup still owns per-thread data and socket release. */
VOID WINAPI wow_user_task_retire_async(void)
{
    current_provider()->retire_async();
}

VOID W32FreeTask(PVOID task)
{
    if (!wow_user_task_prepare_exit())
        wow_user_native_call_failed("prepare-task-resource-exit", GetLastError());
    current_provider()->free_task(task);
}
