#include "include/mvdm_command_launch_request.h"
#include "include/console_services.h"
#include "include/mvdm_command_native_child.h"

#include <string.h>

void ntvdm_command_launch_destroy(ntvdm_command_launch *request)
{
    unsigned int i;
    if(!request)return;
    for(i=0;i<3;++i)if(request->streams[i])CloseHandle(request->streams[i]);
    if(request->command)HeapFree(GetProcessHeap(),0,request->command);
    if(request->environment)HeapFree(GetProcessHeap(),0,request->environment);
    if(request->directory)HeapFree(GetProcessHeap(),0,request->directory);
    HeapFree(GetProcessHeap(),0,request);
}

DWORD ntvdm_command_launch_prepare(unsigned int service,const char *command,
    const char *environment,size_t environment_bytes,const char *directory,
    const HANDLE streams[3],ntvdm_command_launch **out)
{
    ntvdm_command_launch *request;
    size_t i;
    DWORD error=ERROR_NOT_ENOUGH_MEMORY,kind;
    if(out)*out=NULL;
    if(!out || !command || !*command || !environment || environment_bytes<2 ||
        !directory || !*directory || !streams ||
        service>MVDM_COMMAND_SERVICE_EXECUTE)return ERROR_INVALID_PARAMETER;
    if(!((directory[0]=='\\' && directory[1]=='\\') ||
        (strlen(directory)>=3 && directory[1]==':' &&
        (directory[2]=='\\' || directory[2]=='/'))))return ERROR_BAD_PATHNAME;
    /* Reject truncated blocks and hidden trailing input after the terminator. */
    for(i=0;i+1<environment_bytes;++i)
        if(!environment[i] && !environment[i+1])break;
    if(i+2!=environment_bytes)return ERROR_INVALID_DATA;
    request=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*request));
    if(!request)return error;
    request->service=service;
    request->command=HeapAlloc(GetProcessHeap(),0,strlen(command)+1);
    request->environment=HeapAlloc(GetProcessHeap(),0,environment_bytes);
    request->directory=HeapAlloc(GetProcessHeap(),0,strlen(directory)+1);
    if(!request->command || !request->environment || !request->directory)goto failed;
    memcpy(request->command,command,strlen(command)+1);
    memcpy(request->directory,directory,strlen(directory)+1);
    memcpy(request->environment,environment,environment_bytes);
    for(i=0;i<3;++i){
        if(!streams[i] || streams[i]==INVALID_HANDLE_VALUE){error=ERROR_INVALID_HANDLE;goto failed;}
        kind=ntvdm_console_handle_kind(streams[i]);
        if(kind && kind!=(i==0 ? 1u : 2u)){error=ERROR_INVALID_HANDLE;goto failed;}
        if(kind)request->console_mask|=1u<<i;
        if(!DuplicateHandle(GetCurrentProcess(),streams[i],GetCurrentProcess(),
            &request->streams[i],0,FALSE,DUPLICATE_SAME_ACCESS)){
            error=GetLastError();goto failed;
        }
    }
    *out=request;return ERROR_SUCCESS;
failed:
    ntvdm_command_launch_destroy(request);return error;
}
