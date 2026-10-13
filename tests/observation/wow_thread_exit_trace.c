/* Diagnostic-only build substitution; no guest writes or exit-policy changes. */
#define opennt_exit_thread observed_original_exit_thread
#include "../../src/ntvdm-exe/win32/thread_start_compat.c"
#undef opennt_exit_thread
#include <stdio.h>

VOID WINAPI opennt_exit_thread(DWORD exit_code)
{
    DWORD saved_error = GetLastError();
    void *frames[24];
    USHORT count = CaptureStackBackTrace(0, 24, frames, NULL), i;
    FILE *file = fopen(S35_ENVIRONMENT_TRACE_LOG, "a");
    if (file) {
        fprintf(file, "pid=%lu tid=%lu exit=%lu error=%lu\n",
            GetCurrentProcessId(), GetCurrentThreadId(), exit_code, saved_error);
        for (i = 0; i < count; ++i) {
            MEMORY_BASIC_INFORMATION info;
            char path[MAX_PATH] = "";
            if (VirtualQuery(frames[i], &info, sizeof(info))) {
                GetModuleFileNameA((HMODULE)info.AllocationBase, path, sizeof(path));
                fprintf(file, "frame=%u pc=%p base=%p offset=%08lx module=%s\n",
                    i, frames[i], info.AllocationBase,
                    (unsigned long)((char *)frames[i]-(char *)info.AllocationBase), path);
            }
        }
        fclose(file);
    }
    SetLastError(saved_error);
    observed_original_exit_thread(exit_code);
}
