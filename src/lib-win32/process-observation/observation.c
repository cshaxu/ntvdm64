#include "observation.h"
/* Native-width ProcessBasicInformation; no PEB or remote-memory access.
 * https://learn.microsoft.com/windows/win32/api/winternl/nf-winternl-ntqueryinformationprocess */
typedef struct win32_process_basic_information {
    LONG exit_status;PVOID peb;ULONG_PTR affinity;LONG priority;
    ULONG_PTR pid,parent;
} win32_process_basic_information;
typedef LONG (NTAPI *win32_process_query)(HANDLE,ULONG,PVOID,ULONG,PULONG);
DWORD win32_process_observation_parent(HANDLE process,ULONG_PTR *parent)
{
    win32_process_basic_information basic={0};ULONG bytes=0;
    win32_process_query query=(win32_process_query)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtQueryInformationProcess");
    if(!parent)return ERROR_INVALID_PARAMETER;
    *parent=0;
    if(!query)return ERROR_CALL_NOT_IMPLEMENTED;
    if(query(process,0,&basic,sizeof(basic),&bytes)<0 || bytes!=sizeof(basic))return ERROR_NOT_SUPPORTED;
    *parent=basic.parent;return ERROR_SUCCESS;
}
BOOL win32_process_observation_retain(HANDLE source,HANDLE *retained)
{
    return DuplicateHandle(GetCurrentProcess(),source,GetCurrentProcess(),retained,
        SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0);
}
void win32_process_observation_identity(HANDLE process,DWORD *pid,FILETIME *created)
{
    FILETIME value,exit_time,kernel,user;
    created->dwLowDateTime=created->dwHighDateTime=0;
    if(GetProcessTimes(process,&value,&exit_time,&kernel,&user))*created=value;
    *pid=GetProcessId(process);
}
BOOL win32_process_observation_image(HANDLE process,WCHAR *image,DWORD capacity)
{
    return QueryFullProcessImageNameW(process,0,image,&capacity);
}
BOOL win32_process_observation_watch(HANDLE *wait,HANDLE process,WAITORTIMERCALLBACK callback,void *context)
{
    return RegisterWaitForSingleObject(wait,process,callback,context,INFINITE,WT_EXECUTEONLYONCE);
}
BOOL win32_process_observation_drain(HANDLE wait)
{
    return UnregisterWaitEx(wait,INVALID_HANDLE_VALUE);
}
