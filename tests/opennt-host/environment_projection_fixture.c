#include <windows.h>
#include <nt.h>
#include <ntrtl.h>
#include "opennt-host/base/ntos/rtl/environapi.h"
#include "adapter-mvdm-host-out/win32/include/mvdm_base_vdm_environment.h"

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
    ok = BaseCreateVDMEnvironment(NULL, &clonedAnsi, &clonedUnicode);
    if (!ok) return 5;
    ok = explicitUnicode.Length == clonedUnicode.Length &&
        memcmp(explicitUnicode.Buffer, clonedUnicode.Buffer,
            explicitUnicode.Length) == 0;
    (void)BaseDestroyVDMEnvironment(&explicitAnsi, &explicitUnicode);
    (void)BaseDestroyVDMEnvironment(&clonedAnsi, &clonedUnicode);
    return ok ? 0 : 6;
}
