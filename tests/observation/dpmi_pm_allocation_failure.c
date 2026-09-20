/* Test-only replacement of the sole allocation call in this owner. */
#define DpmiAllocateVirtualMemory s38_fail_pm_allocation
#define DpmiPassPmStackInfo s38_original_pm_publication
#include "../../src/mvdm/dpmi32/dpmiint.c"
#undef DpmiPassPmStackInfo
#undef DpmiAllocateVirtualMemory

static void record_failure(const char *text)
{
    DWORD written;
    HANDLE log = CreateFileA(DPMI_TRACE_LOG, FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (log != INVALID_HANDLE_VALUE) {
        WriteFile(log, text, (DWORD)lstrlenA(text), &written, NULL);
        CloseHandle(log);
    }
}

NTSTATUS s38_fail_pm_allocation(PVOID *address, PULONG size)
{
    char text[128];
    wsprintfA(text, "S38_PM_ALLOCATION_REFUSED size=%lu address=%lu\r\n",
              *size, (ULONG)*address);
    record_failure(text);
    return (NTSTATUS)-1;
}

VOID DpmiPassPmStackInfo(VOID)
{
    s38_original_pm_publication();
    record_failure("S38_FAIL_PM_PUBLICATION_RETURNED\r\n");
}
