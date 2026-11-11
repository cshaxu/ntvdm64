/* The test EXE stays below build, while using the ordinary runtime package.
 * Only this translation unit's self-relative discovery is redirected. No
 * production object or process authentication uses this test substitution. */
#include <windows.h>
#include <wchar.h>
static DWORD test_module_path(HMODULE module,LPWSTR output,DWORD capacity)
{
    WCHAR root[MAX_PATH];
    DWORD length=GetEnvironmentVariableW(L"MVDM_TEST_PACKAGE_ROOT",root,MAX_PATH);
    if(module || !length) return GetModuleFileNameW(module,output,capacity);
    if(length>=MAX_PATH || length+11>capacity) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);return 0;
    }
    memcpy(output,root,length*sizeof(WCHAR));
    memcpy(output+length,L"\\run16.exe",11*sizeof(WCHAR));
    return length+10;
}
#define GetModuleFileNameW test_module_path
#include "../../src/run16-exe/main.c"
