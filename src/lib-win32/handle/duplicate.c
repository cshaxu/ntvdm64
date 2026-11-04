#include "duplicate.h"
win32_handle_compare_fn win32_handle_compare_provider(void)
{
    return (win32_handle_compare_fn)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
}
DWORD win32_handle_duplicate(HANDLE source_process,HANDLE source,HANDLE target_process,HANDLE *output)
{
    if(!output)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    if(!DuplicateHandle(source_process,source,target_process,output,0,FALSE,DUPLICATE_SAME_ACCESS))
        return GetLastError();
    return ERROR_SUCCESS;
}
void win32_handle_close_remote(HANDLE owner_process,HANDLE remote_handle)
{
    HANDLE local=NULL;
    if(remote_handle && DuplicateHandle(owner_process,remote_handle,GetCurrentProcess(),&local,0,FALSE,
        DUPLICATE_CLOSE_SOURCE|DUPLICATE_SAME_ACCESS))CloseHandle(local);
}
