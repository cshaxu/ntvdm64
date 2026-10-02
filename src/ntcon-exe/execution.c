/* Recovered from NTKVM native_console_request.c. NTCON creates the native
 * target on its own Console; the requester receives that actual process. */
#include "execution.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "io.h"
#include "interface/native_launch.h"
#include "interface/frontend_protocol.h"
struct ntcon_executions {
    CRITICAL_SECTION lock;
    HANDLE stop,idle,broker_failed;
    DWORD active,broker_error;
    ntcon_execution_io io;
    ntcon_execution_fault fault;
    void *fault_context;
};
typedef struct ntcon_execution {
    ntcon_executions *owner;
    HANDLE root_capability,pipe,sender,execution,event;
    DWORD request,preflight_error;
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
/* Before the serving thread exists, the caller still owns the broker command
 * attachments.  Keep that common worker-base failure contract intact: the
 * caller completes and disposes the command exactly once. */
static void release_unstarted_request(ntcon_execution *request)
{
    if(!request)return;
    request->root_capability=NULL;
    request->pipe=NULL;
    request->sender=NULL;
    request->execution=NULL;
    request->request=0;
    release_request(request);
}
static void finish_request(ntcon_executions *owner)
{
    EnterCriticalSection(&owner->lock);
    if(!--owner->active)SetEvent(owner->idle);
    LeaveCriticalSection(&owner->lock);
}
void ntcon_executions_note_broker_failure(ntcon_executions *owner,DWORD error)
{
    ntcon_execution_fault fault;
    void *context;
    if(!owner || !error)return;
    EnterCriticalSection(&owner->lock);
    if(!owner->broker_error)owner->broker_error=error;
    fault=owner->fault;context=owner->fault_context;
    SetEvent(owner->broker_failed);
    LeaveCriticalSection(&owner->lock);
    if(fault)fault(context,error);
}
static DWORD launch_request(ntcon_execution *request,BYTE *payload,DWORD bytes,
    HANDLE receipt,HANDLE *target)
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
        if(!error)error=run16_native_launch_start_suspended(local_payload,local_bytes,&process);
        /* Bind the direct target identity before it can execute.  S24 does
         * not assign a Job here: descendant observation is deferred and must
         * never decide direct-command admission or completion. */
        if(!error)error=OpenNtBaseClientBindNativeTarget(request->request,process.hProcess,receipt);
        if(!error && ResumeThread(process.hThread)==(DWORD)-1)error=GetLastError();
        if(local_payload)HeapFree(GetProcessHeap(),0,local_payload);
        if(process.hThread)CloseHandle(process.hThread);
        if(!error)*target=process.hProcess;
        else if(process.hProcess) {
            /* The target has not been handed off until ResumeThread succeeds.
             * This is the one permitted startup rollback, never a tree kill. */
            (void)TerminateProcess(process.hProcess,error ? error : ERROR_OPERATION_ABORTED);
            CloseHandle(process.hProcess);
        }
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
    BOOL bound=FALSE,broker_completed=FALSE;
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
    if(header.version!=NATIVE_REQUEST_VERSION || header.bytes<sizeof(run16_native_launch_packet) ||
        header.bytes>NATIVE_LAUNCH_MAX_BYTES) {
        reply.error=ERROR_INVALID_DATA;
    } else {
        payload=HeapAlloc(GetProcessHeap(),0,header.bytes);
        if(!payload)reply.error=ERROR_NOT_ENOUGH_MEMORY;
        else {
            error=ntcon_channel_transfer(request->pipe,request->sender,owner->stop,request->event,FALSE,payload,header.bytes);
            if(error)goto done;
            /* The client writes its whole request before reading the reply.
             * Consume that payload first, then return an explicit preflight
             * failure on the established channel rather than completing the
             * broker record and leaving run16 blocked on its reply. */
            if(request->preflight_error) {
                reply.error=request->preflight_error;
                goto reply_ready;
            }
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
            if(!reply.error)reply.error=launch_request(request,payload,header.bytes,receipt,&target);
            if(bound && owner->io.release_launch)owner->io.release_launch(owner->io.context);
            if(!reply.error && !DuplicateHandle(GetCurrentProcess(),target,request->sender,&remote,
                SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0))reply.error=GetLastError();
            if(!reply.error)reply.request=request->request;
            reply.target=(uint64_t)(ULONG_PTR)remote;
            reply.receipt=reply.error ? 0 : (uint64_t)(ULONG_PTR)remote_receipt;
        }
    }
reply_ready:
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
            DWORD broker_error,exit_code=0;
            if(!GetExitCodeProcess(target,&exit_code)) {
                error=GetLastError();
                ntcon_executions_note_broker_failure(owner,error);
                goto done;
            }
            error=bound ? owner->io.end(owner->io.context) : ERROR_SUCCESS;
            bound=FALSE;
            completion.error=error;
            /* Report only the real target's exit code after native I/O release.
             * NTSRV stores it and signals the launcher's direct receipt. */
            broker_error=ntcon_complete_next_command(request->request,exit_code);
            if(broker_error) {
                ntcon_executions_note_broker_failure(owner,broker_error);
                error=broker_error;
                goto done;
            }
            broker_completed=TRUE;
            (void)ntcon_channel_transfer(request->pipe,request->sender,owner->stop,request->event,
                TRUE,&completion,sizeof(completion));
        }
    }
done:
    if(bound)(void)owner->io.end(owner->io.context);
    if(receipt)CloseHandle(receipt);
    if(target)CloseHandle(target);
    if(payload)HeapFree(GetProcessHeap(),0,payload);
    if(!broker_completed){
        if(target) {
            /* A running target has not completed. Worker rundown, not a fake
             * success receipt, must fail the broker's outstanding record. */
            ntcon_executions_note_broker_failure(owner,error ? error : ERROR_PROCESS_ABORTED);
        } else {
            DWORD completion_error=ntcon_complete_next_command(request->request,0);
            if(completion_error)ntcon_executions_note_broker_failure(owner,completion_error);
        }
    }
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
    owner->broker_failed=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!owner->stop || !owner->idle || !owner->broker_failed) {
        error=GetLastError();
        if(owner->stop)CloseHandle(owner->stop);
        if(owner->idle)CloseHandle(owner->idle);
        if(owner->broker_failed)CloseHandle(owner->broker_failed);
        HeapFree(GetProcessHeap(),0,owner);return error;
    }
    InitializeCriticalSection(&owner->lock);
    *output=owner;return ERROR_SUCCESS;
}
void ntcon_executions_bind_io(ntcon_executions *owner,const ntcon_execution_io *io)
{ owner->io=*io; }
void ntcon_executions_bind_fault(ntcon_executions *owner,ntcon_execution_fault fault,void *context)
{ owner->fault=fault;owner->fault_context=context; }
BOOL ntcon_executions_idle(ntcon_executions *owner)
{ return WaitForSingleObject(owner->idle,0)==WAIT_OBJECT_0; }
DWORD ntcon_executions_wait_idle(ntcon_executions *owner)
{
    DWORD wait,error;
    if(!owner)return ERROR_INVALID_PARAMETER;
    /* Test/shutdown drain, not the production admission gate: nested CMD
     * requires GetNext while an earlier direct target is still active.
     * A latched completion failure wins over idle for callers draining. */
    wait=WaitForSingleObject(owner->idle,INFINITE);
    if(wait==WAIT_OBJECT_0 && WaitForSingleObject(owner->broker_failed,0)==WAIT_OBJECT_0) {
        EnterCriticalSection(&owner->lock);error=owner->broker_error;LeaveCriticalSection(&owner->lock);
        return error ? error : ERROR_BROKEN_PIPE;
    }
    return wait==WAIT_OBJECT_0 ? ERROR_SUCCESS :
        wait==WAIT_FAILED ? GetLastError() : ERROR_OPERATION_ABORTED;
}
DWORD ntcon_execution_start(ntcon_executions *owner,ntcon_next_command *command,DWORD preflight_error)
{
    ntcon_execution *request;
    DWORD error;HANDLE thread;
    /* request zero is the original broker's I/O-resume channel. The wire
     * header distinguishes it from a Direct target after GetNext. */
    if(!owner || !command)return ERROR_INVALID_PARAMETER;
    request=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*request));
    if(!request)return ERROR_NOT_ENOUGH_MEMORY;
    request->owner=owner;request->root_capability=command->frontend;
    request->pipe=command->channel;request->sender=command->sender;
    request->execution=command->execution;request->request=command->request;
    request->preflight_error=preflight_error;
    request->event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!request->event) { error=GetLastError();release_unstarted_request(request);return error; }
    EnterCriticalSection(&owner->lock);
    if(WaitForSingleObject(owner->stop,0)!=WAIT_TIMEOUT) {
        LeaveCriticalSection(&owner->lock);release_unstarted_request(request);return ERROR_OPERATION_ABORTED;
    }
    if(!owner->active)ResetEvent(owner->idle);
    ++owner->active;
    LeaveCriticalSection(&owner->lock);
    thread=CreateThread(NULL,0,serve,request,0,NULL);
    if(!thread) { error=GetLastError();release_unstarted_request(request);finish_request(owner);return error; }
    /* Only the live serving thread owns these handles from this point. */
    ZeroMemory(command,sizeof(*command));
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
    DeleteCriticalSection(&owner->lock);
    CloseHandle(owner->broker_failed);CloseHandle(owner->idle);CloseHandle(owner->stop);
    HeapFree(GetProcessHeap(),0,owner);
}
