/* Short-lived run16 role selected by the S1 Console design. Never selects
 * BaseSrv records. Only its parent's private pipe carries query/results. */
#include <windows.h>
#include "basesrv-exe/transport/console_probe.h"
#include "basesrv-exe/transport/console_membership.h"

static DWORD transfer(HANDLE pipe,void *data,DWORD length,BOOL write)
{
    DWORD done;
    BYTE *cursor=data;
    while (length) {
        BOOL ok=write ? WriteFile(pipe,cursor,length,&done,NULL) :
                       ReadFile(pipe,cursor,length,&done,NULL);
        if (!ok) return GetLastError();
        if (!done) return ERROR_BROKEN_PIPE;
        cursor+=done; length-=done;
    }
    return 0;
}

DWORD app_console_probe(void)
{
    /* AttachConsole can replace standard handles: retain the inherited pipe
     * handles before calling the observation mechanism. Parent bounds lifetime. */
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE), output=GetStdHandle(STD_OUTPUT_HANDLE);
    BROKER_CONSOLE_PROBE_REQUEST request;
    BROKER_CONSOLE_PROBE_REPLY reply={BROKER_CONSOLE_PROBE_VERSION,0,0,0};
    DWORD *candidates=NULL,error,current;
    BYTE *members=NULL;
    if (GetConsoleProcessList(&current,1)) return ERROR_ACCESS_DENIED;
    if (GetFileType(input)!=FILE_TYPE_PIPE || GetFileType(output)!=FILE_TYPE_PIPE)
        return ERROR_INVALID_HANDLE;
    error=transfer(input,&request,sizeof(request),FALSE);
    if (error) return error;
    if (request.version!=BROKER_CONSOLE_PROBE_VERSION || request.reserved ||
        !request.caller || request.count>BROKER_CONSOLE_PROBE_MAX_CANDIDATES)
        reply.status=ERROR_INVALID_DATA;
    else {
        if (request.count) {
            candidates=HeapAlloc(GetProcessHeap(),0,request.count*sizeof(*candidates));
            members=HeapAlloc(GetProcessHeap(),0,request.count);
            if (!candidates || !members) {reply.status=ERROR_NOT_ENOUGH_MEMORY;goto respond;}
            error=transfer(input,candidates,request.count*sizeof(*candidates),FALSE);
            if (error) {reply.status=error;goto respond;}
        }
        reply.status=broker_console_membership(request.caller,candidates,request.count,members);
        if (!reply.status) reply.count=request.count;
    }
respond:
    error=transfer(output,&reply,sizeof(reply),TRUE);
    if (!error && reply.count) error=transfer(output,members,reply.count,TRUE);
    if (members) HeapFree(GetProcessHeap(),0,members);
    if (candidates) HeapFree(GetProcessHeap(),0,candidates);
    return error ? error : reply.status;
}
