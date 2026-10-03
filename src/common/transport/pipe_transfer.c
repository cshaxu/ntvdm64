#include "pipe_transfer.h"
#include <string.h>

static DWORD checked_count(DWORD requested,DWORD count)
{
    return !count || count>requested ? ERROR_BROKEN_PIPE : ERROR_SUCCESS;
}
DWORD common_pipe_begin(common_pipe_operation *operation,HANDLE pipe,HANDLE event,
    BOOL write,void *buffer,DWORD bytes,DWORD capacity,DWORD *transferred)
{
    BOOL ok;DWORD error;
    if(!operation || !transferred || !pipe || pipe==INVALID_HANDLE_VALUE ||
        !event || event==INVALID_HANDLE_VALUE || !buffer || !bytes || bytes>capacity)
        return ERROR_INVALID_PARAMETER;
    *transferred=0;
    ZeroMemory(operation,sizeof(*operation));
    operation->pipe=pipe;operation->requested=bytes;operation->io.hEvent=event;
    if(!ResetEvent(event))return GetLastError();
    ok=write ? WriteFile(pipe,buffer,bytes,transferred,&operation->io) :
        ReadFile(pipe,buffer,bytes,transferred,&operation->io);
    if(ok)return checked_count(bytes,*transferred);
    error=GetLastError();
    operation->pending=error==ERROR_IO_PENDING;
    return error;
}
DWORD common_pipe_finish(common_pipe_operation *operation,DWORD *transferred)
{
    DWORD error;
    if(!operation || !operation->pending || !transferred)return ERROR_INVALID_STATE;
    *transferred=0;
    if(!GetOverlappedResult(operation->pipe,&operation->io,transferred,FALSE)) {
        error=GetLastError();
        /* An incomplete result still owns the caller's buffer and event. */
        if(error!=ERROR_IO_INCOMPLETE)operation->pending=FALSE;
        return error;
    }
    operation->pending=FALSE;
    return checked_count(operation->requested,*transferred);
}
void common_pipe_cancel_drain(common_pipe_operation *operation)
{
    DWORD transferred;
    if(!operation || !operation->pending)return;
    (void)CancelIoEx(operation->pipe,&operation->io);
    (void)GetOverlappedResult(operation->pipe,&operation->io,&transferred,TRUE);
    operation->pending=FALSE;
}
DWORD common_pipe_transfer(HANDLE pipe,HANDLE peer,HANDLE cancel,HANDLE event,
    common_pipe_priority priority,DWORD peer_error,BOOL write,void *buffer,
    DWORD bytes,DWORD capacity)
{
    BYTE *cursor=buffer;
    if(!pipe || pipe==INVALID_HANDLE_VALUE || !peer || peer==INVALID_HANDLE_VALUE ||
        !event || event==INVALID_HANDLE_VALUE || !peer_error ||
        (priority!=COMMON_PIPE_COMPLETION_FIRST && priority!=COMMON_PIPE_PEER_DEATH_FIRST) ||
        bytes>capacity || (!buffer && bytes))return ERROR_INVALID_PARAMETER;
    while(bytes) {
        common_pipe_operation operation;
        HANDLE waits[3];DWORD transferred=0,error,wait,completed;
        if(cancel && WaitForSingleObject(cancel,0)==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
        if(priority==COMMON_PIPE_PEER_DEATH_FIRST && WaitForSingleObject(peer,0)!=WAIT_TIMEOUT)
            return peer_error;
        error=common_pipe_begin(&operation,pipe,event,write,cursor,bytes,capacity,&transferred);
        if(error==ERROR_IO_PENDING) {
            completed=priority==COMMON_PIPE_COMPLETION_FIRST ? 0 : 1;
            waits[completed]=event;waits[1-completed]=peer;waits[2]=cancel;
            wait=WaitForMultipleObjects(cancel ? 3 : 2,waits,FALSE,INFINITE);
            if(wait!=WAIT_OBJECT_0+completed) {
                error=wait==WAIT_FAILED ? GetLastError() :
                    wait==WAIT_OBJECT_0+2 ? ERROR_OPERATION_ABORTED : peer_error;
            }else error=common_pipe_finish(&operation,&transferred);
            common_pipe_cancel_drain(&operation);
        }
        if(error)return error;
        cursor+=transferred;bytes-=transferred;capacity-=transferred;
    }
    return ERROR_SUCCESS;
}
