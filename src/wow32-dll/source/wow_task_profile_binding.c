#include "wow_task_profile_bindings.h"

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
