/* Deterministic thread-set changes around the experimental late guard. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "lib-win32/process-hook/late_guard.h"

static HANDLE ready, release_thread, created;
static DWORD callback_error;

static DWORD WINAPI worker(void *unused)
{
    (void)unused;
    if (!SetEvent(ready)) return GetLastError();
    return WaitForSingleObject(release_thread, 10000) == WAIT_OBJECT_0 ? 0 : 1;
}

static void create_during_guard(void)
{
    created = CreateThread(NULL, 0, worker, NULL, 0, NULL);
    if (!created) callback_error = GetLastError();
    else if (WaitForSingleObject(ready, 5000) != WAIT_OBJECT_0)
        callback_error = ERROR_TIMEOUT;
}

static int exercise(BOOL after_suspend)
{
    process_hook_late_guard guard = { 0 };
    DWORD result, exit_code = 1;
    ready = CreateEventW(NULL, TRUE, FALSE, NULL);
    release_thread = CreateEventW(NULL, TRUE, FALSE, NULL);
    created = NULL;
    callback_error = 0;
    if (!ready || !release_thread) return 1;
    if (after_suspend) process_hook_late_guard_test_after_suspend(create_during_guard);
    else process_hook_late_guard_test_before_suspend(create_during_guard);
    result = process_hook_late_guard_begin(&guard, NULL, 0);
    process_hook_late_guard_test_before_suspend(NULL);
    process_hook_late_guard_test_after_suspend(NULL);
    if (result == ERROR_SUCCESS) (void)process_hook_late_guard_end(&guard);
    if (created) {
        SetEvent(release_thread);
        if (WaitForSingleObject(created, 5000) == WAIT_OBJECT_0)
            GetExitCodeThread(created, &exit_code);
        CloseHandle(created);
    }
    CloseHandle(ready);
    CloseHandle(release_thread);
    if (callback_error || !created || exit_code || result != ERROR_RETRY ||
            guard.threads || guard.count || guard.suspended || guard.snapshots) {
        fprintf(stderr, "FAIL after=%d result=%lu callback=%lu worker=%lu\n",
            after_suspend, result, callback_error, exit_code);
        return 1;
    }
    printf("PASS new thread %s suspension rejected and guard released\n",
        after_suspend ? "after" : "before");
    return 0;
}

int main(void)
{
    if (exercise(FALSE) || exercise(TRUE)) return 1;
    return 0;
}
