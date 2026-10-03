/* Recovered from NTCON native_console_request.c. NTVWM creates the native
 * target on its own Console; the requester receives that actual process. */
#include "execution.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "console_state.h"
#include "run16-exe/native_launch.h"
#include "common/protocol/frontend_protocol.h"
#include "worker-base/connection.h"
struct ntvwm_executions {
    CRITICAL_SECTION lock;
    HANDLE stop,idle,broker_failed;
    DWORD active,broker_error;
    ntvwm_execution_io io;
    ntvwm_execution_fault fault;
    void *fault_context;
};
typedef struct ntvwm_execution {
    ntvwm_executions *owner;
    HANDLE root_capability,sender,execution;
    BYTE *payload;
    DWORD bytes;
    DWORD request,preflight_error,caller_generation;
} ntvwm_execution;
static void release_request(ntvwm_execution *request)
{
    if(request->payload)HeapFree(GetProcessHeap(),0,request->payload);
    if(request->sender)CloseHandle(request->sender);
    if(request->execution)CloseHandle(request->execution);
    if(request->root_capability)CloseHandle(request->root_capability);
    HeapFree(GetProcessHeap(),0,request);
}
/* Before the serving thread exists, the caller still owns the broker command
 * attachments.  Keep that common worker-base failure contract intact: the
 * caller completes and disposes the command exactly once. */
static void release_unstarted_request(ntvwm_execution *request)
{
    if(!request)return;
    request->root_capability=NULL;
    request->payload=NULL;
    request->sender=NULL;
    request->execution=NULL;
    request->request=0;
    release_request(request);
}
static void finish_request(ntvwm_executions *owner)
{
    EnterCriticalSection(&owner->lock);
    if(!--owner->active)SetEvent(owner->idle);
    LeaveCriticalSection(&owner->lock);
}
void ntvwm_executions_note_broker_failure(ntvwm_executions *owner,DWORD error)
{
    ntvwm_execution_fault fault;
    void *context;
    if(!owner || !error)return;
    EnterCriticalSection(&owner->lock);
    if(!owner->broker_error)owner->broker_error=error;
    fault=owner->fault;context=owner->fault_context;
    SetEvent(owner->broker_failed);
    LeaveCriticalSection(&owner->lock);
    if(fault)fault(context,error);
}
static DWORD launch_request(ntvwm_execution *request,BYTE *payload,DWORD bytes,
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
    start.directory=strings[2];start.environment=strings[3];
    start.console_mask=request->root_capability ? header.console_mask : 0;
    start.capabilities[0]=request->root_capability;start.capabilities[1]=request->execution;
    for(i=0;i<3;++i) {
        HANDLE source;
        if(header.standard[i]>(uint64_t)(ULONG_PTR)-1) { error=ERROR_INVALID_HANDLE;goto done; }
        source=(HANDLE)(ULONG_PTR)header.standard[i];
        if(header.console_mask&(1u<<i)) {
            /* GUI inherits redirected files/pipes but no text Console or
             * frontend capability from its launcher/worker. */
            start.standard[i]=NULL;continue;
        }
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
    ntvwm_execution *request=context;
    ntvwm_executions *owner=request->owner;
    DWORD startup_status=ERROR_SUCCESS;
    BYTE *payload=request->payload;HANDLE target=NULL,receipt=NULL;DWORD error=ERROR_SUCCESS;
    BOOL bound=FALSE,broker_completed=FALSE,startup_reported=FALSE;
    if(!request->bytes) {
        startup_status=owner->io.begin ? owner->io.begin(owner->io.context,owner->stop) : ERROR_NOT_SUPPORTED;
        if(!startup_status) {
            if(owner->io.release_launch)owner->io.release_launch(owner->io.context);
            startup_status=owner->io.resume ? owner->io.resume(owner->io.context) : ERROR_NOT_SUPPORTED;
        }
        error=OpenNtBaseClientNativeStartupResult(request->caller_generation,request->request,
            startup_status,NULL,NULL);
        startup_reported=TRUE;
        if(error)ntvwm_executions_note_broker_failure(owner,error);
        goto done;
    }
    if(!payload || request->bytes<sizeof(run16_native_launch_packet) ||
        request->bytes>NATIVE_LAUNCH_MAX_BYTES) {
        startup_status=ERROR_INVALID_DATA;
    } else {
        {
            /* Validate copied command data, then publish preflight
             * failure through RPC before completing its broker record. */
            if(request->preflight_error) {
                startup_status=request->preflight_error;
                goto reply_ready;
            }
            /* Prepare the completion object before target side effects.
             * Its typed binding is authenticated before ResumeThread. */
            receipt=CreateEventW(NULL,TRUE,FALSE,NULL);
            if(!receipt)startup_status=GetLastError();
            if(!startup_status) {
                startup_status=worker_base_start_character_io(request->root_capability!=NULL,
                    owner->io.begin,owner->io.context,owner->stop);
                bound=request->root_capability!=NULL && !startup_status;
            }
            if(!startup_status)startup_status=launch_request(request,payload,request->bytes,receipt,&target);
            if(bound && owner->io.release_launch)owner->io.release_launch(owner->io.context);
        }
    }
reply_ready:
    error=OpenNtBaseClientNativeStartupResult(request->caller_generation,request->request,
        startup_status,startup_status ? NULL : target,startup_status ? NULL : receipt);
    startup_reported=TRUE;
    if(error)ntvwm_executions_note_broker_failure(owner,error);
    if(!error && !startup_status && !request->root_capability) {
        /* NTSRV now owns the GUI process reference and real exit watch.
         * Startup released this worker request; no local GUI wait or I/O. */
        broker_completed=TRUE;
        goto done;
    }
    if(!error && !startup_status){
        HANDLE waits[2]={target,owner->stop};
        if(WaitForMultipleObjects(2,waits,FALSE,INFINITE)==WAIT_OBJECT_0) {
            DWORD broker_error,exit_code=0,io_flags=0;
            if(!GetExitCodeProcess(target,&exit_code)) {
                error=GetLastError();
                ntvwm_executions_note_broker_failure(owner,error);
                goto done;
            }
            error=bound ? owner->io.end(owner->io.context) : ERROR_SUCCESS;
            bound=FALSE;
            /* One resource check after the direct process has exited. Do not
             * close an owned Console still used by an unregistered native
             * child. Failure is conservative; no timer or observed task is
             * introduced. The broker, not this worker, owns retirement. */
            if(ntvwm_console_quiescent(GetProcessId(target)))
                io_flags=NATIVE_COMPLETION_CONSOLE_EMPTY;
            /* Report only the real target's exit code after native I/O release.
             * NTSRV stores it and signals the launcher's direct receipt. */
            broker_error=ntvwm_complete_native_request(request->request,exit_code,error,io_flags);
            if(broker_error) {
                ntvwm_executions_note_broker_failure(owner,broker_error);
                error=broker_error;
                goto done;
            }
            broker_completed=TRUE;
        }
    }
done:
    if(!startup_reported) {
        DWORD startup_error=OpenNtBaseClientNativeStartupResult(request->caller_generation,
            request->request,error ? error : ERROR_PROCESS_ABORTED,NULL,NULL);
        if(startup_error)ntvwm_executions_note_broker_failure(owner,startup_error);
    }
    if(bound)(void)owner->io.end(owner->io.context);
    if(receipt)CloseHandle(receipt);
    if(target)CloseHandle(target);
    if(!broker_completed){
        if(target) {
            /* A running target has not completed. Worker rundown, not a fake
             * success receipt, must fail the broker's outstanding record. */
            ntvwm_executions_note_broker_failure(owner,error ? error : ERROR_PROCESS_ABORTED);
        } else {
            DWORD completion_error=ntvwm_complete_next_command(request->request,0);
            if(completion_error)ntvwm_executions_note_broker_failure(owner,completion_error);
        }
    }
    release_request(request);
    finish_request(owner);
    return error;
}
DWORD ntvwm_executions_open(ntvwm_executions **output)
{
    ntvwm_executions *owner;DWORD error;
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
void ntvwm_executions_bind_io(ntvwm_executions *owner,const ntvwm_execution_io *io)
{ owner->io=*io; }
void ntvwm_executions_bind_fault(ntvwm_executions *owner,ntvwm_execution_fault fault,void *context)
{ owner->fault=fault;owner->fault_context=context; }
BOOL ntvwm_executions_idle(ntvwm_executions *owner)
{ return WaitForSingleObject(owner->idle,0)==WAIT_OBJECT_0; }
DWORD ntvwm_executions_wait_idle(ntvwm_executions *owner)
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
DWORD ntvwm_execution_start(ntvwm_executions *owner,ntvwm_next_command *command,DWORD preflight_error)
{
    ntvwm_execution *request;
    DWORD error;HANDLE thread;
    /* A zero-length command is the existing broker I/O-resume request,
     * not a Direct target. */
    if(!owner || !command)return ERROR_INVALID_PARAMETER;
    request=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*request));
    if(!request)return ERROR_NOT_ENOUGH_MEMORY;
    request->owner=owner;request->root_capability=command->frontend;
    request->payload=command->payload;request->bytes=command->bytes;request->sender=command->sender;
    request->execution=command->execution;request->request=command->request;
    request->caller_generation=command->caller_generation;
    request->preflight_error=preflight_error;
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
void ntvwm_executions_close(ntvwm_executions *owner)
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
