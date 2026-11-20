#include "include/wow_user_failure.h"
#include <stdio.h>

/* Observe the existing retirement edge independently of ModuleUnload and
 * launcher completion. Default-off; preserve the caller's error status. */
void WINAPI wow_user_retirement_trace(const char *stage, DWORD task, DWORD status)
{
    char path[MAX_PATH], line[160];
    DWORD error = GetLastError(), size, written, path_length;
    HANDLE file;
    path_length = GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", path, sizeof(path));
    if (path_length && path_length < sizeof(path)) {
        file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            size = (DWORD)sprintf_s(line, sizeof(line),
                "%lu TaskRetirement tid=%lu task=%08lX stage=%s status=%lu\r\n",
                GetCurrentProcessId(), GetCurrentThreadId(), task, stage, status);
            if (size) (void)WriteFile(file, line, size, &written, NULL);
            CloseHandle(file);
        }
    }
    SetLastError(error);
}

/* A native operation may already have consumed a MSG or mutated USER. It
 * cannot be replayed, reported as an ordinary false result or followed by
 * unsafe guest execution. Fail the worker, not merely the current WOW task.
 * Use the product's existing fail-fast convention for broken ownership. */
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *stage, DWORD error)
{
    EXCEPTION_RECORD record = {0};
    wow_user_retirement_trace(stage, 0, error);
    record.ExceptionCode = 0xc0000420u; /* STATUS_ASSERTION_FAILURE */
    record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
    record.NumberParameters = 1;
    record.ExceptionInformation[0] = error;
    RaiseFailFastException(&record, NULL, 0);
    __assume(0);
}
