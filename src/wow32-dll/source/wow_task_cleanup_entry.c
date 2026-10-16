#include "precomp.h"
#include "wow_task_cleanup_entry.h"
#include "wow_user_runtime.h"
#include "wow_task_order_bindings.h"

extern PTD gptdTaskHead;
extern CRITICAL_SECTION gcsWOW;

BOOL WINAPI wow_user_task_cleanup_uninitialized(DWORD task_id)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    PTD task, current = CURRENTPTD();
    BOOL matches = FALSE;
    DWORD error = GetLastError();
    /* W32Thread publishes hThread only after InitTask succeeds. A failed
     * attempt has no USER carrier, but its original live TD still identifies
     * the terminal W32DestroyTask call. Never accept a foreign/stale TEB. */
    if (!task_id || task_id > 0xffff || !binding || !binding->runtime ||
            binding->thread || binding->classes || !gptdTaskHead) return FALSE;
    EnterCriticalSection(&gcsWOW);
    for (task = gptdTaskHead; task; task = task->ptdNext) {
        if (task == current) {
            matches = task->htask16 == task_id && !task->hThread &&
                task->dwThreadID == GetCurrentThreadId();
            break;
        }
    }
    LeaveCriticalSection(&gcsWOW);
    SetLastError(error);
    return matches;
}

/* The cleanup call retains its opaque pointer ABI and original /Gz convention;
 * wkman.c retains the complete task/resource cleanup implementation.
 * Normal task windows are already retired by SG_EXIT's pfnWOWCleanup,
 * before guest GlobalFreeAll. This is the fallback for original remote-task
 * and incomplete-startup exits which do not necessarily send that signal. */
VOID WowOriginalFreeTask(PVOID);

VOID W32FreeTask(PVOID task)
{
    if (!wow_user_task_prepare_exit())
        wow_user_native_call_failed("prepare-task-resource-exit", GetLastError());
    WowOriginalFreeTask(task);
}
