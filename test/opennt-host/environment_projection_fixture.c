#include <windows.h>
#include <nt.h>
#include <ntrtl.h>
#include "compat/include/environapi.h"
#include "ntvdm-exe/win32/mvdm_base_vdm_environment.h"

BOOL BaseCreateVDMEnvironment(PWCHAR environment, ANSI_STRING *ansi,
    UNICODE_STRING *unicode);
BOOL BaseDestroyVDMEnvironment(ANSI_STRING *ansi, UNICODE_STRING *unicode);

int main(void)
{
    LPWCH current;
    ANSI_STRING explicitAnsi;
    ANSI_STRING clonedAnsi;
    UNICODE_STRING explicitUnicode;
    UNICODE_STRING clonedUnicode;
    BOOL ok;
    MEMORY_BASIC_INFORMATION memory;
    PVOID explicitAllocation, clonedAllocation;

    if (mvdm_base_vdm_environment_prepare(NULL) ||
        GetLastError() != ERROR_INVALID_PARAMETER) return 1;

    ZeroMemory(&explicitAnsi, sizeof(explicitAnsi));
    ZeroMemory(&clonedAnsi, sizeof(clonedAnsi));
    ZeroMemory(&explicitUnicode, sizeof(explicitUnicode));
    ZeroMemory(&clonedUnicode, sizeof(clonedUnicode));
    current = GetEnvironmentStringsW();
    if (current == NULL) return 2;
    ok = BaseCreateVDMEnvironment(current, &explicitAnsi, &explicitUnicode);
    FreeEnvironmentStringsW(current);
    if (!ok) return 3;
    if (!OpenNtRtlRefreshEnvironmentSnapshot()) return 4;
    RtlAcquirePebLock();
    ok = NtCurrentPeb()->FastPebLock->OwningThread == NtCurrentTeb()->ClientId.UniqueThread;
    RtlReleasePebLock();
    if (!ok) return 7;
    ok = BaseCreateVDMEnvironment(NULL, &clonedAnsi, &clonedUnicode);
    if (!ok) return 5;
    ok = explicitUnicode.Length == clonedUnicode.Length &&
        memcmp(explicitUnicode.Buffer, clonedUnicode.Buffer,
            explicitUnicode.Length) == 0;
    explicitAllocation = explicitUnicode.Buffer;
    clonedAllocation = clonedUnicode.Buffer;
    if (!VirtualQuery(explicitAllocation, &memory, sizeof(memory)) ||
        memory.Type != MEM_PRIVATE || memory.AllocationBase != explicitAllocation) return 8;
    (void)BaseDestroyVDMEnvironment(&explicitAnsi, &explicitUnicode);
    (void)BaseDestroyVDMEnvironment(&clonedAnsi, &clonedUnicode);
    if (!VirtualQuery(explicitAllocation, &memory, sizeof(memory)) ||
        memory.State != MEM_FREE) return 9;
    if (!VirtualQuery(clonedAllocation, &memory, sizeof(memory)) ||
        memory.State != MEM_FREE) return 10;
    SetLastError(ERROR_SUCCESS);
    if (BaseCreateVDMEnvironment(NULL, NULL, &explicitUnicode) ||
        GetLastError() != ERROR_INVALID_PARAMETER) return 11;
    return ok ? 0 : 6;
}
