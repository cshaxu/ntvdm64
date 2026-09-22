#include "wow_task_profile_bindings.h"

/* ADAPTER-WOW-042: original PMAP_COMPAT is machine-wide, never impersonated
 * HKCU configuration. Select the native machine view explicitly on WOW64;
 * do not create the absent key or synthesize compatibility values. */
HANDLE WINAPI wow_profile_open_key(UINT section, ACCESS_MASK access)
{
    HKEY key=NULL;
    if(section!=14 || access!=KEY_READ) return NULL;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Compatibility",
        0,KEY_READ|KEY_WOW64_64KEY,&key)!=ERROR_SUCCESS) return NULL;
    return key;
}

UINT WINAPI wow_task_profile(wow_task_order_process *process,UINT section,
    LPCWSTR key,LPCWSTR fallback,LPWSTR output,DWORD capacity)
{
    (void)process;
    /* Fixed family and valid caller buffer are this adapter's contract.
     * Default/copy/query policy remains in original FastGetProfileStringW. */
    if(section!=14 || !key || !output || !capacity ||
        capacity>(MAXDWORD-sizeof(KEY_VALUE_PARTIAL_INFORMATION))/sizeof(WCHAR) ||
        (fallback && wcslen(fallback)>=capacity)) {
        SetLastError(ERROR_INVALID_PARAMETER);return 0;
    }
    return FastGetProfileStringW(section,key,fallback,output,capacity);
}
