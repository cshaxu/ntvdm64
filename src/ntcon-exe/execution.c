/* Recovered from NTKVM native_console_request.c. NTCON creates the native
 * target on its own Console; the requester receives that actual process. */
#include "execution.h"
#include "io.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "interface/native_launch.h"
#include "interface/frontend_protocol.h"
struct ntcon_executions {
    CRITICAL_SECTION lock;
    HANDLE stop,idle;
    DWORD active;
    ntcon_execution_io io;
};
typedef struct ntcon_execution {
    ntcon_executions *owner;
    HANDLE root_capability,pipe,sender,execution,event;
    DWORD request;
} ntcon_execution;
static void release_request(ntcon_execution *request)
{
    if(request->pipe)CloseHandle(request->pipe);
    if(request->sender)CloseHandle(request->sender);
    if(request->execution)CloseHandle(request->execution);
    if(request->event)CloseHandle(request->event);
    if(request->root_capability)CloseHandle(request->root_capability);
    HeapFree(GetProcessHeap(),0,request);
}
static void finish_request(ntcon_executions *owner)
{
    EnterCriticalSection(&owner->lock);
    if(!--owner->active)SetEvent(owner->idle);
    LeaveCriticalSection(&owner->lock);
}
static DWORD launch_request(ntcon_execution *request,BYTE *payload,DWORD bytes,HANDLE *target)
{
    run16_native_launch_packet header;
    run16_native_start start={0};
    WCHAR *strings[4];HANDLE local[3]={0};DWORD i,j,error;
    error=run16_native_launch_unpack(payload,bytes,&header,strings);
    if(error)return error;
    /* These authorities come only from broker attachments, not wire numbers. */
    if(header.capabilities[0] || header.capabilities[1])return ERROR_INVALID_DATA;
    start.application=*strings[0] ? strings[0] : NULL;start.command=strings[1];
    start.directory=strings[2];start.environment=strings[3];start.console_mask=header.console_mask;
    start.capabilities[0]=request->root_capability;start.capabilities[1]=request->execution;
    for(i=0;i<3;++i) {
        HANDLE source;
        if(header.standard[i]>(uint64_t)(ULONG_PTR)-1) { error=ERROR_INVALID_HANDLE;goto done; }
        source=(HANDLE)(ULONG_PTR)header.standard[i];
        if(header.console_mask&(1u<<i))continue;
        if(!source || source==INVALID_HANDLE_VALUE) { start.standard[i]=source;continue; }
        for(j=0;j<i;++j)if(header.standard[j]==header.standard[i] && local[j])break;
        if(j<i)local[i]=local[j];
        else if(!DuplicateHandle(request->sender,source,GetCurrentProcess(),&local[i],
            0,FALSE,DUPLICATE_SAME_ACCESS)) { error=GetLastError();goto done; }
        if(GetFileType(local[i])!=FILE_TYPE_DISK && GetFileType(local[i])!=FILE_TYPE_PIPE &&
            GetFileType(local[i])!=FILE_TYPE_CHAR) {
            error=ERROR_INVALID_HANDLE;goto done;
        }
        start.standard[i]=local[i];
    }
    {
        BYTE *local_payload=NULL;DWORD local_bytes=0;PROCESS_INFORMATION process={0};
        error=run16_native_launch_pack(&start,&local_payload,&local_bytes);
        if(!error)error=run16_native_launch_start(local_payload,local_bytes,&process);
        if(local_payload)HeapFree(GetProcessHeap(),0,local_payload);
        if(process.hThread)CloseHandle(process.hThread);
        if(!error)*target=process.hProcess;
        else if(process.hProcess)CloseHandle(process.hProcess);
    }
done:
    for(i=0;i<3;++i)if(local[i]) {
        for(j=0;j<i;++j)if(local[j]==local[i])break;
        if(j==i)CloseHandle(local[i]);
    }
    return error;
}
static DWORD WINAPI serve(void *context)
{
    ntcon_execution *request=context;
    ntcon_executions *owner=request->owner;
    native_request_header header;
    native_request_reply reply={NATIVE_REQUEST_VERSION,0,0};
    BYTE *payload=NULL;HANDLE target=NULL,remote=NULL,receipt=NULL,remote_receipt=NULL;DWORD error;
    BOOL bound=FALSE;
    error=ntcon_channel_transfer(request->pipe,request->sender,owner->stop,request->event,FALSE,&header,sizeof(header));
    if(error)goto done;
    if(header.version==NATIVE_REQUEST_VERSION && !header.bytes) {
        reply.error=owner->io.begin ? owner->io.begin(owner->io.context,owner->stop) : ERROR_NOT_SUPPORTED;
        if(!reply.error) {
            if(owner->io.release_launch)owner->io.release_launch(owner->io.context);
            reply.error=owner->io.end ? owner->io.end(owner->io.context) : ERROR_SUCCESS;
        }
        error=ntcon_channel_transfer(request->pipe,request->sender,owner->stop,request->event,
            TRUE,&reply,sizeof(reply));
        goto done;
    }
    if(header.version!=NATIVE_REQUEST_VERSION || header.bytes<sizeof(run16_native_launch_packet)) {
        reply.error=ERROR_INVALID_DATA;
    } else {
        payload=HeapAlloc(GetProcessHeap(),0,header.bytes);
        if(!payload)reply.error=ERROR_NOT_ENOUGH_MEMORY;
        else {
            error=ntcon_channel_transfer(request->pipe,request->sender,owner->stop,request->event,FALSE,payload,header.bytes);
            if(error)goto done;
            /* Prepare the completion export before allowing target side
             * effects. A denied export must not start an unreportable task. */
            receipt=CreateEventW(NULL,TRUE,FALSE,NULL);
            if(!receipt)reply.error=GetLastError();
            if(!reply.error && !DuplicateHandle(GetCurrentProcess(),receipt,request->sender,&remote_receipt,
                SYNCHRONIZE,FALSE,0))reply.error=GetLastError();
            if(!reply.error && owner->io.begin) {
                reply.error=owner->io.begin(owner->io.context,owner->stop);
                bound=!reply.error;
            }
            if(!reply.error)reply.error=launch_request(request,payload,header.bytes,&target);
            if(bound && owner->io.release_launch)owner->io.release_launch(owner->io.context);
            if(!reply.error && !DuplicateHandle(GetCurrentProcess(),target,request->sender,&remote,
                SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0))reply.error=GetLastError();
            reply.target=(uint64_t)(ULONG_PTR)remote;
            reply.receipt=reply.error ? 0 : (uint64_t)(ULONG_PTR)remote_receipt;
        }
    }
    error=ntcon_channel_transfer(request->pipe,request->sender,owner->stop,request->event,TRUE,&reply,sizeof(reply));
    if(error && remote) {
        HANDLE copy=NULL;
        /* An incomplete response cannot be consumed. Reclaim only our export
         * copy, never the handed-off target process or any descendant. */
        if(DuplicateHandle(request->sender,remote,GetCurrentProcess(),&copy,
            0,FALSE,DUPLICATE_SAME_ACCESS|DUPLICATE_CLOSE_SOURCE))CloseHandle(copy);
    }
    if((error || reply.error) && remote_receipt){
        HANDLE copy=NULL;
        if(DuplicateHandle(request->sender,remote_receipt,GetCurrentProcess(),&copy,
            0,FALSE,DUPLICATE_SAME_ACCESS|DUPLICATE_CLOSE_SOURCE))CloseHandle(copy);
    }
    if(!error && !reply.error){
        HANDLE waits[2]={target,owner->stop};
        if(WaitForMultipleObjects(2,waits,FALSE,INFINITE)==WAIT_OBJECT_0) {
            native_request_completion completion={NATIVE_REQUEST_VERSION,0};
            error=bound ? owner->io.end(owner->io.context) : ERROR_SUCCESS;
            bound=FALSE;
            completion.error=error;
            (void)ntcon_channel_transfer(request->pipe,request->sender,owner->stop,request->event,
                TRUE,&completion,sizeof(completion));
            SetEvent(receipt);
        }
    }
done:
    if(bound)(void)owner->io.end(owner->io.context);
    if(receipt)CloseHandle(receipt);
    if(target)CloseHandle(target);
    if(payload)HeapFree(GetProcessHeap(),0,payload);
    (void)OpenNtBaseClientCompleteWorkerChannel(request->request);
    release_request(request);
    finish_request(owner);
    return error;
}
DWORD ntcon_executions_open(ntcon_executions **output)
{
    ntcon_executions *owner;DWORD error;
    if(!output)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    owner=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*owner));
    if(!owner)return ERROR_NOT_ENOUGH_MEMORY;
    owner->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    owner->idle=CreateEventW(NULL,TRUE,TRUE,NULL);
    if(!owner->stop || !owner->idle) {
        error=GetLastError();
        if(owner->stop)CloseHandle(owner->stop);
        if(owner->idle)CloseHandle(owner->idle);
        HeapFree(GetProcessHeap(),0,owner);return error;
    }
    InitializeCriticalSection(&owner->lock);*output=owner;return ERROR_SUCCESS;
}
void ntcon_executions_bind_io(ntcon_executions *owner,const ntcon_execution_io *io)
{ owner->io=*io; }
BOOL ntcon_executions_idle(ntcon_executions *owner)
{ return WaitForSingleObject(owner->idle,0)==WAIT_OBJECT_0; }
DWORD ntcon_execution_start(ntcon_executions *owner,HANDLE root_capability,HANDLE channel,
    HANDLE sender,HANDLE execution,DWORD request_id)
{
    ntcon_execution *request=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*request));
    DWORD error;HANDLE thread;
    if(!request) { CloseHandle(root_capability);CloseHandle(channel);CloseHandle(sender);CloseHandle(execution);return ERROR_NOT_ENOUGH_MEMORY; }
    request->owner=owner;request->root_capability=root_capability;
    request->pipe=channel;request->sender=sender;request->execution=execution;
    request->request=request_id;
    request->event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!request->event) { error=GetLastError();release_request(request);return error; }
    EnterCriticalSection(&owner->lock);
    if(WaitForSingleObject(owner->stop,0)!=WAIT_TIMEOUT) {
        LeaveCriticalSection(&owner->lock);release_request(request);return ERROR_OPERATION_ABORTED;
    }
    if(!owner->active)ResetEvent(owner->idle);
    ++owner->active;
    LeaveCriticalSection(&owner->lock);
    thread=CreateThread(NULL,0,serve,request,0,NULL);
    if(!thread) { error=GetLastError();release_request(request);finish_request(owner);return error; }
    CloseHandle(thread);return ERROR_SUCCESS;
}
void ntcon_executions_close(ntcon_executions *owner)
{
    if(!owner)return;
    EnterCriticalSection(&owner->lock);SetEvent(owner->stop);LeaveCriticalSection(&owner->lock);
    WaitForSingleObject(owner->idle,INFINITE);
    /* Last cleanup signals idle under lock; do not destroy the lock until it
     * has finished touching the owner. No target process is terminated. */
    EnterCriticalSection(&owner->lock);LeaveCriticalSection(&owner->lock);
    DeleteCriticalSection(&owner->lock);CloseHandle(owner->idle);CloseHandle(owner->stop);
    HeapFree(GetProcessHeap(),0,owner);
}
