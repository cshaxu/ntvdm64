#include "observation.h"
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
