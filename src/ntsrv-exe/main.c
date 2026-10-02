/* Standalone BaseSrv composition. S3 integration in progress: the current
 * endpoint offers registration/first-VDM and typed stream receipts, not DOS launch.
 * It is build-only until command/resource/worker and idle gates are complete. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include "service.h"
#include "ntsrv-exe/transport/rpc_security.h"
#include "opennt-abi/source/public/internal/base/inc/vdmapi.h"
#include "ntsrv-exe/opennt/include/base_service.h"
#include "interface/version.h"
static broker_rpc_scope scope;
static OPENNT_BASE_SERVICE *service;
static SRWLOCK idle_lock=SRWLOCK_INIT;
static HANDLE idle_timer;
static HANDLE frontend_timer; /* Broker-only startup and workerless-root deadlines. */
static ULONGLONG idle_deadline;
static ULONG pending_connects;
error_status_t Server_SubmitNativeRequest(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,HANDLE frontend,ULONG bytes,BYTE *payload,
    HANDLE *target,HANDLE *receipt,ULONG *request)
{
    DWORD pid,error;
    if(!target || !receipt || !request)return ERROR_INVALID_PARAMETER;
    *target=*receipt=NULL;*request=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceSubmitNativeRequest(connection,pid,generation,
        frontend,bytes,payload,target,receipt,request);
}
error_status_t Server_FinishNativeRequest(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,ULONG request,ULONG *exit_code,ULONG *target_completed)
{
    DWORD pid,error;
    if(!exit_code || !target_completed)return ERROR_INVALID_PARAMETER;
    *exit_code=*target_completed=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceFinishNativeRequest(connection,pid,generation,
        request,exit_code,target_completed);
}
error_status_t Server_StartVdmWorker(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,ULONG characters,WCHAR *environment,
    ULONG show,HANDLE frontend,HANDLE *worker,HANDLE *parent,ULONG *receipt)
{
    DWORD pid,error;
    if(!worker || !parent || !receipt)return ERROR_INVALID_PARAMETER;
    *worker=*parent=NULL;*receipt=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceStartVdmWorker(connection,pid,generation,
        characters,environment,show,frontend,worker,parent,receipt);
}
error_status_t Server_StartNativeWorker(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,HANDLE *worker)
{
    DWORD pid,error;
    if(!worker)return ERROR_INVALID_PARAMETER;
    *worker=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceStartNativeWorker(connection,pid,generation,worker);
}
error_status_t Server_StartFrontend(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,LONGLONG window,ULONG borrowed,
    HANDLE *root,HANDLE *capability,HANDLE *restored)
{
    DWORD pid,error;
    if(!root || !capability || !restored)return ERROR_INVALID_PARAMETER;
    *root=*capability=*restored=NULL;
    if(borrowed>1)return ERROR_INVALID_PARAMETER;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceStartFrontend(connection,pid,generation,
        (uint64_t)window,borrowed!=0,root,capability,restored);
}
error_status_t Server_ReturnFrontendConsole(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceReturnFrontendConsole(connection,pid,generation);
}
error_status_t Server_FrontendConsoleRestored(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceFrontendConsoleRestored(connection,pid,generation);
}
error_status_t Server_WaitFrontendConsoleRestored(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,unsigned long generation)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceWaitFrontendConsoleRestored(connection,pid,generation);
}
static BOOL idle_stopping;
#define BASESRV_EMPTY_GRACE_MS 10000u
#define BASE_CHECK_REPLY_BYTES 40u
#define BASE_UPDATE_REPLY_BYTES 32u

static __declspec(noreturn) void basesrv_idle_fatal(PCSTR operation,DWORD error)
{
    if (!error) error=ERROR_GEN_FAILURE;
    fprintf(stderr,"ntsrv: fatal %s: %lu\n",operation,error); fflush(stderr);
    /* Broken retention/stop machinery cannot leave a silently resident broker.
     * Do not run DLL teardown while another RPC thread may hold its locks. */
    TerminateProcess(GetCurrentProcess(),error);
    abort();
}
/* idle_lock serializes Connect admission with the final empty decision.
 * Management observations never cancel or restart this ten-second grace. */
static void basesrv_schedule_empty_locked(void)
{
    LARGE_INTEGER due;
    if (!idle_stopping && !idle_deadline && !pending_connects &&
        service && OpenNtBaseServiceIsEmpty(service)) {
        due.QuadPart=-(LONGLONG)BASESRV_EMPTY_GRACE_MS*10000;
        idle_deadline=GetTickCount64()+BASESRV_EMPTY_GRACE_MS;
        if (!SetWaitableTimer(idle_timer,&due,0,NULL,NULL,FALSE))
            basesrv_idle_fatal("SetWaitableTimer",GetLastError());
    }
}
static void basesrv_schedule_empty_stop(void)
{
    AcquireSRWLockExclusive(&idle_lock);
    basesrv_schedule_empty_locked();
    ReleaseSRWLockExclusive(&idle_lock);
}
static DWORD basesrv_begin_connect(void)
{
    DWORD error=ERROR_SUCCESS;
    AcquireSRWLockExclusive(&idle_lock);
    if (idle_stopping) error=RPC_S_SERVER_UNAVAILABLE;
    else {
        if (pending_connects==MAXDWORD) basesrv_idle_fatal("Connect count",ERROR_ARITHMETIC_OVERFLOW);
        ++pending_connects;
        idle_deadline=0;
        if (!CancelWaitableTimer(idle_timer))
            basesrv_idle_fatal("CancelWaitableTimer",GetLastError());
    }
    ReleaseSRWLockExclusive(&idle_lock);
    return error;
}
static void basesrv_end_connect(void)
{
    AcquireSRWLockExclusive(&idle_lock);
    --pending_connects;
    basesrv_schedule_empty_locked();
    ReleaseSRWLockExclusive(&idle_lock);
}
static BOOL basesrv_claim_empty_stop(void)
{
    BOOL stop=FALSE;
    ULONGLONG now;
    LARGE_INTEGER due;
    AcquireSRWLockExclusive(&idle_lock);
    now=GetTickCount64();
    if (!pending_connects && idle_deadline) {
        if (now<idle_deadline) {
            /* A wake consumed just before Connect rearmed the same timer. */
            due.QuadPart=-(LONGLONG)(idle_deadline-now)*10000;
            if (!SetWaitableTimer(idle_timer,&due,0,NULL,NULL,FALSE))
                basesrv_idle_fatal("SetWaitableTimer",GetLastError());
        } else if (OpenNtBaseServiceIsEmpty(service)) {
            idle_stopping=TRUE;
            stop=TRUE;
        } else idle_deadline=0;
    }
    ReleaseSRWLockExclusive(&idle_lock);
    return stop;
}
static DWORD basesrv_management_version(ULONG protocol,const unsigned char application_version[32])
{
    static const unsigned char expected[APP_VERSION_BYTES]=APP_VERSION;
    if (protocol!=APP_PROTOCOL_VERSION || !application_version ||
        memcmp(application_version,expected,sizeof(expected))) return ERROR_REVISION_MISMATCH;
    return ERROR_SUCCESS;
}
/* The service invokes this only after the retained, authenticated worker
 * process has actually exited and the original BaseSrv cleanup has run. */
static void WINAPI basesrv_worker_terminated(void *context)
{
    (void)context;
    basesrv_schedule_empty_stop();
}
error_status_t Server_AttachFile(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG role,HANDLE stream,ULONG *receipt)
{
    DWORD pid,error;
    *receipt=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    return OpenNtBaseServiceAttachStream(connection,pid,generation,role,stream,receipt);
}
error_status_t Server_AttachPipe(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG role,HANDLE stream,ULONG *receipt)
{
    return Server_AttachFile(binding,connection,process,generation,role,stream,receipt);
}
error_status_t Server_RevokeStream(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG receipt)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    return OpenNtBaseServiceRevokeStream(connection,pid,generation,receipt);
}
error_status_t Server_TaskSnapshot(handle_t binding,HANDLE process,ULONG protocol,
    unsigned char application_version[32],ULONG *count,DTASKMGR_WORKER **entries)
{
    OPENNT_BASE_WORKER_INFO *local=NULL;
    DWORD pid,error;
    uint32_t actual=0,index;
    uint64_t service_epoch=0;
    if (!count || !entries) return ERROR_INVALID_PARAMETER;
    *count=0; *entries=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=basesrv_management_version(protocol,application_version);
    if (!error) error=OpenNtBaseServiceSnapshot(service,&service_epoch,NULL,0,&actual);
    if (error!=ERROR_INSUFFICIENT_BUFFER && error!=ERROR_SUCCESS) goto done;
    if (actual) {
        local=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*local)*actual);
        if (!local) { error=ERROR_NOT_ENOUGH_MEMORY; goto done; }
        error=OpenNtBaseServiceSnapshot(service,&service_epoch,local,actual,&actual);
        if (error) goto done;
        *entries=MIDL_user_allocate(sizeof(**entries)*actual);
        if (!*entries) { error=ERROR_NOT_ENOUGH_MEMORY; goto done; }
        for (index=0;index<actual;++index) {
            (*entries)[index].process_id=local[index].process_id;
            (*entries)[index].kind=local[index].kind;
            (*entries)[index].state=local[index].state;
            (*entries)[index].started_filetime=(hyper)local[index].started_filetime;
            (*entries)[index].task=local[index].task;
            (*entries)[index].stack_depth=local[index].stack_depth;
            memcpy((*entries)[index].image,local[index].image,sizeof(local[index].image));
        }
    }
    *count=actual;
done:
    if (local) HeapFree(GetProcessHeap(),0,local);
    if (error && *entries) { MIDL_user_free(*entries); *entries=NULL; }
    if (error) *count=0;
    return error;
}
error_status_t Server_TerminateWorker(handle_t binding,HANDLE process,ULONG protocol,
    unsigned char application_version[32],ULONG process_id)
{
    DWORD pid,error;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=basesrv_management_version(protocol,application_version);
    if (!error) error=OpenNtBaseServiceTerminateWorker(service,process_id);
    basesrv_schedule_empty_stop();
    return error;
}
void *__RPC_USER midl_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER midl_user_free(void *value) { free(value); }
void __RPC_USER VDM_CONNECTION_rundown(VDM_CONNECTION connection)
{
    /* No tasks yet; disconnect also drains retained input stream receipts. */
    if (OpenNtBaseServiceDisconnect(connection)) RaiseFailFastException(NULL,NULL,0);
    basesrv_schedule_empty_stop();
    fputs("basesrv: connection rundown completed\n",stderr); fflush(stderr);
}
static RPC_STATUS RPC_ENTRY authorize(RPC_IF_HANDLE interfaceId,void *binding)
{
    (void)interfaceId;
    return broker_rpc_authorize(&scope,binding);
}
/* RPC [out, system_handle] consumes its server-side handle.  Service
 * receipts remain owned by BaseSrv; export only independent duplicates. */
static DWORD export_handles(const HANDLE *source,ULONG count,HANDLE **output)
{
    ULONG index;
    DWORD error;
    *output=NULL;
    if (!count) return ERROR_SUCCESS;
    *output=MIDL_user_allocate(sizeof(**output)*count);
    if (!*output) return ERROR_NOT_ENOUGH_MEMORY;
    for (index=0;index<count;++index) {
        (*output)[index]=NULL;
        if (!source[index]) continue;
        if (!DuplicateHandle(GetCurrentProcess(),source[index],GetCurrentProcess(),
                &(*output)[index],0,FALSE,DUPLICATE_SAME_ACCESS)) {
            error=GetLastError();
            while (index) { --index; if ((*output)[index]) CloseHandle((*output)[index]); }
            MIDL_user_free(*output); *output=NULL;
            return error;
        }
    }
    return ERROR_SUCCESS;
}
error_status_t Server_Connect(handle_t binding,HANDLE process,ULONG protocol,
    unsigned char application_version[32],ULONG *server_protocol,
    unsigned char server_version[32],VDM_CONNECTION *connection,ULONG *generation)
{
    static const unsigned char expected[APP_VERSION_BYTES]=APP_VERSION;
    DWORD pid;
    RPC_STATUS status;
    *connection=NULL; *generation=0;
    *server_protocol=APP_PROTOCOL_VERSION;
    memcpy(server_version,expected,sizeof(expected));
    status=basesrv_begin_connect();
    if (status) return status;
    /* Every admitted attempt, including identity/version failure or an RPC
     * exception, releases its transient hold and rechecks empty retention. */
    __try {
        status=broker_rpc_peer_process(&scope,binding,process,&pid);
        if (status) __leave;
        if (protocol!=APP_PROTOCOL_VERSION || memcmp(application_version,expected,sizeof(expected))) {
            fprintf(stderr,"basesrv: version mismatch: local protocol=%u app=%s; peer protocol=%lu app=%.32s\n",
                APP_PROTOCOL_VERSION,APP_VERSION,protocol,(const char *)application_version);
            status=ERROR_REVISION_MISMATCH;
            __leave;
        }
        status=OpenNtBaseServiceConnect(service,process,(OPENNT_BASE_CONNECTION **)connection,generation);
    } __finally {
        basesrv_end_connect();
    }
    return status;
}
error_status_t Server_ReportConsoleMembers(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,ULONG count,ULONG *members)
{
    DWORD pid;
    RPC_STATUS status=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (status) return status;
    return OpenNtBaseServiceReportConsoleMembers(connection,pid,generation,count,members);
}
error_status_t Server_BrokerProcess(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE *server)
{
    DWORD pid,error;
    *server=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_ACCESS_DENIED;
    /* RPC consumes this non-inheritable identity/wait duplicate. The receiver
     * can verify a bootstrap pipe's server PID but cannot terminate, modify
     * or duplicate resources from the broker through this capability. */
    if (!DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
            server,SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0)) return GetLastError();
    return ERROR_SUCCESS;
}
error_status_t Server_GetNextNativeCommand(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE *channel,HANDLE *caller_process,HANDLE *execution,HANDLE *frontend,ULONG *request)
{
    DWORD pid,error;
    *channel=NULL;*caller_process=NULL;*execution=NULL;*frontend=NULL;*request=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceGetNextNativeCommand(connection,pid,generation,
        channel,caller_process,execution,frontend,request);
}
error_status_t Server_WorkerFrontendCapability(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE *capability)
{
    DWORD pid,error;
    *capability=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceWorkerFrontendCapability(connection,pid,generation,capability);
}
error_status_t Server_AcquireConsoleContext(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE frontend,HANDLE *capability)
{
    DWORD pid,error;
    *capability=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceAcquireConsoleContext(connection,pid,generation,
        frontend,capability);
}
error_status_t Server_BindConsoleContext(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE capability)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceBindConsoleContext(connection,pid,generation,capability);
}
error_status_t Server_RegisterNativeBackend(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE frontend,HANDLE stop,HANDLE closed)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceRegisterNativeBackend(connection,pid,generation,frontend,stop,closed);
}

error_status_t Server_WorkerShutdownEvent(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE *shutdown)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    *shutdown=NULL;
    return error ? error : OpenNtBaseServiceWorkerShutdownEvent(connection,pid,generation,shutdown);
}
error_status_t Server_CompleteWorkerChannel(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG request,ULONG exit_code)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceCompleteWorkerChannel(connection,pid,generation,request,exit_code);
}
error_status_t Server_NativeExitCode(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG request,ULONG *exit_code)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceNativeExitCode(connection,pid,generation,request,exit_code);
}
error_status_t Server_BindNativeTarget(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG request,HANDLE target,HANDLE receipt)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceBindNativeTarget(connection,pid,generation,request,target,receipt);
}
error_status_t Server_RegisterFrontendRoot(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE capability)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceRegisterFrontendRoot(connection,pid,generation,capability);
}
error_status_t Server_AcquireFrontendRoot(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,LONGLONG console_window,ULONG *create_root,HANDLE *root,HANDLE *capability,
    HANDLE *retire,HANDLE *restored)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if(error)return error;
    return OpenNtBaseServiceAcquireFrontendRoot(connection,pid,generation,(uint64_t)console_window,
        create_root,root,capability,retire,restored);
}
error_status_t Server_CancelFrontendRootReservation(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceCancelFrontendRootReservation(connection,pid,generation);
}
error_status_t Server_RegisterFrontendLease(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,LONGLONG console_window,ULONG creator_pid,ULONG borrowed,
    HANDLE retire,HANDLE restored)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceRegisterFrontendLease(connection,pid,generation,
        (uint64_t)console_window,creator_pid,borrowed!=0,retire,restored);
}
error_status_t Server_FrontendJoinCandidate(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,ULONG *nonce,ULONG *candidate_pid)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceFrontendJoinCandidate(connection,pid,generation,
        nonce,candidate_pid);
}
error_status_t Server_FrontendJoinDecision(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,ULONG nonce,ULONG same_console)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceFrontendJoinDecision(connection,pid,generation,
        nonce,same_console!=0);
}
error_status_t Server_FrontendLeaseReady(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceFrontendLeaseReady(connection,pid,generation);
}
error_status_t Server_RetainFrontendRoot(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE capability,HANDLE *root,ULONG *root_generation)
{
    DWORD pid,error;
    *root=NULL;*root_generation=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceRetainFrontendRoot(connection,pid,generation,
        capability,root,root_generation);
}
error_status_t Server_FrontendUsage(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG *pending,ULONG *tasks)
{
    DWORD pid,error;
    if(!pending || !tasks)return ERROR_INVALID_PARAMETER;
    *pending=*tasks=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceFrontendUsage(connection,pid,generation,pending,tasks);
}
error_status_t Server_RetireWorkerlessFrontend(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,ULONG *retired)
{
    DWORD pid,error;
    if(!retired)return ERROR_INVALID_PARAMETER;
    *retired=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceRetireWorkerlessFrontend(connection,pid,generation,retired);
}
error_status_t Server_FrontendStateChanged(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE *state_changed)
{
    DWORD pid,error;
    if(!state_changed)return ERROR_INVALID_PARAMETER;
    *state_changed=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceFrontendStateChanged(connection,pid,generation,state_changed);
}
error_status_t Server_WorkerStateChanged(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE *state_changed)
{
    DWORD pid,error;
    if(!state_changed)return ERROR_INVALID_PARAMETER;
    *state_changed=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceWorkerStateChanged(connection,pid,generation,state_changed);
}
error_status_t Server_RetireFrontend(handle_t binding,VDM_CONNECTION connection,HANDLE process,ULONG generation)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceRetireFrontend(connection,pid,generation);
}
error_status_t Server_RequestFrontend(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE capability)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceRequestFrontend(connection,pid,generation,capability);
}
error_status_t Server_FrontendRequest(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG *request,HANDLE *worker)
{
    DWORD pid,error;
    *request=0;*worker=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceFrontendRequest(connection,pid,generation,request,worker);
}
error_status_t Server_AttachFrontendRequest(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG request,HANDLE channel,HANDLE ready)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if(error) return error;
    error=OpenNtBaseServiceAttachFrontendRequest(connection,pid,generation,request,channel,ready);
    return error;
}
error_status_t Server_SelectNativeWorker(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE *worker)
{
    DWORD pid,error;
    *worker=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceSelectNativeWorker(connection,pid,generation,worker);
}
error_status_t Server_CommandWorker(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE *worker)
{
    DWORD pid,error;
    *worker=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    /* The typed RPC attachment consumes the restricted service duplicate. */
    return OpenNtBaseServiceRetainCommandWorker(connection,pid,generation,worker);
}
error_status_t Server_AttachFrontend(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE pipe,HANDLE ready)
{
    DWORD pid,error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceAttachFrontend(connection,pid,generation,pipe,ready);
}
error_status_t Server_TakeFrontend(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE *pipe,HANDLE *frontend,ULONG *frontend_generation,HANDLE *ready)
{
    DWORD pid,error;
    *pipe=NULL; *frontend=NULL; *frontend_generation=0;*ready=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceTakeFrontend(connection,pid,generation,
        pipe,frontend,frontend_generation,ready);
}
error_status_t Server_WaitFrontend(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE *pipe,HANDLE *frontend,ULONG *frontend_generation,HANDLE *ready)
{
    DWORD pid,error;
    *pipe=NULL;*frontend=NULL;*frontend_generation=0;*ready=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    return error ? error : OpenNtBaseServiceWaitFrontend(connection,pid,generation,
        pipe,frontend,frontend_generation,ready);
}
error_status_t Server_First(handle_t binding,VDM_CONNECTION connection,HANDLE process,ULONG generation,ULONG *first)
{
    DWORD pid;
    RPC_STATUS status;
    *first=0;
    status=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (status) return status;
    status=OpenNtBaseServiceFirst(connection,pid,generation,first);
    return status;
}
error_status_t Server_RegisterWowExec(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,ULONG window)
{
    DWORD pid;
    RPC_STATUS status=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (status) return status;
    return OpenNtBaseServiceRegisterWowExec(connection,pid,generation,window);
}
error_status_t Server_WowStarted(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,ULONG task)
{
    DWORD pid;
    RPC_STATUS status=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (status) return status;
    return OpenNtBaseServiceWowStarted(connection,pid,generation,task);
}
error_status_t Server_WowStartup(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,ULONG parent_receipt,HANDLE *event,ULONG *started)
{
    DWORD pid;
    BOOL value=FALSE;
    RPC_STATUS status;
    *event=NULL; *started=0;
    status=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (status) return status;
    /* Service returns an owned wait-only duplicate; RPC consumes that copy. */
    status=OpenNtBaseServiceWowStartup(connection,pid,generation,parent_receipt,event,&value);
    if (!status) *started=value ? 1u : 0u;
    return status;
}
error_status_t Server_Check(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG requestBytes,unsigned char *request,ULONG *parentEventCount,
    HANDLE **parentEvents,ULONG *parentReceipt,ULONG *replyBytes,
    unsigned char reply[BASE_CHECK_REPLY_BYTES])
{
    DWORD pid,error;
    uint32_t required=0;
    uint32_t parent_receipt=0;
    HANDLE parent_event=NULL;
    if (!parentEventCount || !parentEvents || !parentReceipt || !replyBytes || !reply) return ERROR_INVALID_PARAMETER;
    *parentEventCount=0;*parentEvents=NULL;*parentReceipt=0;*replyBytes=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceCheck(connection,pid,generation,request,requestBytes,
        reply,BASE_CHECK_REPLY_BYTES,&required,&parent_event,&parent_receipt);
    if (!error && required!=BASE_CHECK_REPLY_BYTES) return ERROR_INVALID_DATA;
    if (!error && parent_event) {
        error=export_handles(&parent_event,1,parentEvents);
        if (error) return error;
        *parentEventCount=1;
    }
    if (!error) *parentReceipt=(ULONG)parent_receipt;
    if (!error) *replyBytes=required;
    return error;
}
error_status_t Server_Get(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG requestBytes,unsigned char *request,ULONG *waitEventCount,HANDLE **waitEvents,
    ULONG *pipeStreamMask,ULONG *pipeStreamCount,HANDLE **pipeStreams,
    ULONG *fileStreamMask,ULONG *fileStreamCount,HANDLE **fileStreams,ULONG *replyBytes,
    unsigned char **reply)
{
    DWORD pid,error;
    uint32_t bytes=0;
    ULONG standard_count=0;
    ULONG index;
    HANDLE pipe_standard[3]={NULL,NULL,NULL};
    HANDLE file_standard[3]={NULL,NULL,NULL};
    void *source_reply=NULL;
    HANDLE wait_event=NULL;
    HANDLE standard[3]={NULL,NULL,NULL};
    if (!waitEventCount || !waitEvents || !pipeStreamMask || !pipeStreamCount || !pipeStreams ||
        !fileStreamMask || !fileStreamCount || !fileStreams || !replyBytes || !reply) return ERROR_INVALID_PARAMETER;
    *waitEventCount=0; *waitEvents=NULL;
    *pipeStreamMask=0; *pipeStreamCount=0; *pipeStreams=NULL;
    *fileStreamMask=0; *fileStreamCount=0; *fileStreams=NULL;
    *replyBytes=0; *reply=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceGet(connection,pid,generation,request,requestBytes,
        &source_reply,&bytes,&wait_event,standard,&standard_count);
    if (error) {
        fprintf(stderr,"basesrv: Get rejected %lu\n",error); fflush(stderr);
        return error;
    }
    if (wait_event) {
        error=export_handles(&wait_event,1,waitEvents);
        if (error) {
            OpenNtBaseServiceReleaseCommandReply(source_reply);
            return error;
        }
        *waitEventCount=1;
    }
    if (!bytes || !(*reply=MIDL_user_allocate(bytes))) {
        if (*waitEvents) { CloseHandle(**waitEvents); MIDL_user_free(*waitEvents);
            *waitEvents=NULL; *waitEventCount=0; }
        OpenNtBaseServiceReleaseCommandReply(source_reply);
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    memcpy(*reply,source_reply,bytes);
    for (index=0;index<standard_count;++index) if (standard[index]) {
        if (GetFileType(standard[index])==FILE_TYPE_PIPE) {
            pipe_standard[*pipeStreamCount]=standard[index];
            *pipeStreamMask|=1u<<index;
            ++*pipeStreamCount;
        } else {
            file_standard[*fileStreamCount]=standard[index];
            *fileStreamMask|=1u<<index;
            ++*fileStreamCount;
        }
    }
    error=export_handles(pipe_standard,*pipeStreamCount,pipeStreams);
    if (!error) error=export_handles(file_standard,*fileStreamCount,fileStreams);
    if (error) {
        ULONG close_index;
        MIDL_user_free(*reply); *reply=NULL;
        if (*pipeStreams) {
            for (close_index=0;close_index<*pipeStreamCount;++close_index)
                if ((*pipeStreams)[close_index]) CloseHandle((*pipeStreams)[close_index]);
            MIDL_user_free(*pipeStreams); *pipeStreams=NULL;
        }
        if (*fileStreams) {
            for (close_index=0;close_index<*fileStreamCount;++close_index)
                if ((*fileStreams)[close_index]) CloseHandle((*fileStreams)[close_index]);
            MIDL_user_free(*fileStreams); *fileStreams=NULL;
        }
        *pipeStreamMask=*pipeStreamCount=*fileStreamMask=*fileStreamCount=0;
        if (*waitEvents) { CloseHandle(**waitEvents); MIDL_user_free(*waitEvents);
            *waitEvents=NULL; *waitEventCount=0; }
        OpenNtBaseServiceReleaseCommandReply(source_reply); return error;
    }
    /* Get consumed reservation ownership.  RPC has independently duplicated
     * each selected resource into the worker, so close the broker's source
     * copies now; do not let a resident broker keep a host-pipe writer open. */
    for (index=0;index<standard_count;++index) {
        ULONG previous;
        if (!standard[index]) continue;
        for (previous=0;previous<index;++previous)
            if (standard[previous]==standard[index]) break;
        if (previous==index) CloseHandle(standard[index]);
    }
    OpenNtBaseServiceReleaseCommandReply(source_reply);
    *replyBytes=bytes;
    return ERROR_SUCCESS;
}
error_status_t Server_Exit(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG isWow,ULONG wowTask,ULONG *closeWorkerWait)
{
    DWORD pid,error;
    BOOL close_wait=FALSE;
    unsigned long close_wait_wire=0;
    if (!closeWorkerWait || isWow>1u) return ERROR_INVALID_PARAMETER;
    *closeWorkerWait=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceExit(connection,pid,generation,(BOOL)isWow,wowTask,&close_wait);
    if (!error) {
        close_wait_wire=close_wait ? 1u : 0u;
        *closeWorkerWait=close_wait_wire;
    }
    return error;
}
error_status_t Server_Update(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG requestBytes,unsigned char *request,ULONG *parentEventCount,HANDLE **parentEvents,
    ULONG *parentReceipt,ULONG *replyBytes,unsigned char reply[BASE_UPDATE_REPLY_BYTES])
{
    DWORD pid,error;
    uint32_t required=0;
    uint32_t parent_receipt=0;
    HANDLE parent_event=NULL;
    if (!parentEventCount || !parentEvents || !parentReceipt || !replyBytes || !reply) return ERROR_INVALID_PARAMETER;
    *parentEventCount=0; *parentEvents=NULL; *parentReceipt=0; *replyBytes=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceUpdate(connection,pid,generation,request,requestBytes,reply,
        BASE_UPDATE_REPLY_BYTES,&required,&parent_event,&parent_receipt);
    if (error) {
        fprintf(stderr,"basesrv: Update rejected %lu\n",error); fflush(stderr);
        return error;
    }
    if (required!=BASE_UPDATE_REPLY_BYTES) return ERROR_INVALID_DATA;
    if (parent_event) {
        error=export_handles(&parent_event,1,parentEvents);
        if (error) return error;
        *parentEventCount=1;
    }
    *parentReceipt=(ULONG)parent_receipt;
    *replyBytes=required;
    return ERROR_SUCCESS;
}
error_status_t Server_ExitCode(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG parentReceipt,ULONG *exitCode)
{
    DWORD pid,error;
    if (!exitCode || !parentReceipt) return ERROR_INVALID_PARAMETER;
    *exitCode=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceExitCode(connection,pid,generation,parentReceipt,exitCode);
    return error;
}
error_status_t Server_Reenter(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG increment)
{
    DWORD pid,error;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceReenter(connection,pid,generation,increment);
    return error;
}
error_status_t Server_Reserve(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG task,ULONG native_worker,hyper *reservation)
{
    DWORD pid,error;
    uint64_t id=0;
    if (!reservation) return ERROR_INVALID_PARAMETER;
    *reservation=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    if(native_worker>1 || (native_worker && task))return ERROR_INVALID_PARAMETER;
    error=native_worker ? OpenNtBaseServiceCreateNativeReservation(connection,pid,generation,&id) :
        OpenNtBaseServiceCreateReservation(connection,pid,generation,task,&id);
    if (!error) *reservation=(hyper)id;
    return error;
}
error_status_t Server_Prepare(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,hyper reservation,HANDLE worker)
{
    DWORD pid,error;
    if (reservation<=0 || !worker || worker==INVALID_HANDLE_VALUE) return ERROR_INVALID_PARAMETER;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServicePrepareWorker(connection,pid,generation,(uint64_t)reservation,worker);
    return error;
}
error_status_t Server_Release(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,hyper reservation)
{
    DWORD pid,error;
    if (reservation<=0) return ERROR_INVALID_PARAMETER;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceReleaseReservation(connection,pid,generation,(uint64_t)reservation);
    if (!error) basesrv_schedule_empty_stop();
    return error;
}
error_status_t Server_Disconnect(handle_t binding,HANDLE process,ULONG generation,VDM_CONNECTION *connection)
{
    DWORD pid,result=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (result) return result;
    if (!OpenNtBaseServicePeer(*connection,pid,generation)) return ERROR_ACCESS_DENIED;
    result=OpenNtBaseServiceDisconnect(*connection);
    if (!result) *connection=NULL;
    if (!result) basesrv_schedule_empty_stop();
    return result;
}
int main(void)
{
    WCHAR endpoint[128];
    RPC_STATUS result;
    if (!broker_rpc_capture_scope(&scope)) return (int)GetLastError();
    swprintf_s(endpoint,128,L"ntvdm-basesrv-%lu-%08lx-%08lx",scope.session,
        (ULONG)scope.logon.HighPart,scope.logon.LowPart);
    /* ncalrpc's endpoint binding is exclusive. Duplicate startup never
     * dispatches or pretends to be ready; caller must authenticate the owner. */
    result=RpcServerUseProtseqEpW((RPC_WSTR)L"ncalrpc",RPC_C_PROTSEQ_MAX_REQS_DEFAULT,(RPC_WSTR)endpoint,NULL);
    if (result) return (int)result;
    result=RpcServerRegisterAuthInfoW(NULL,RPC_C_AUTHN_WINNT,NULL,NULL);
    if (result) return (int)result;
    service=OpenNtBaseServiceStart();
    if (!service) return ERROR_NOT_ENOUGH_MEMORY;
    if (!OpenNtBaseServiceConfigureEmptyNotify(service,basesrv_worker_terminated,NULL)) {
        DWORD error=GetLastError();
        (void)OpenNtBaseServiceStop(service);
        return (int)error;
    }
    result=RpcServerRegisterIf3(Server_vdm_service_v31_0_s_ifspec,NULL,NULL,
        RPC_IF_ALLOW_SECURE_ONLY | RPC_IF_ALLOW_LOCAL_ONLY,RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        (unsigned)-1,authorize,NULL);
    if (!result) {
        idle_timer=CreateWaitableTimerW(NULL,FALSE,NULL);
        if (!idle_timer) basesrv_idle_fatal("CreateWaitableTimer",GetLastError());
        frontend_timer=CreateWaitableTimerW(NULL,FALSE,NULL);
        if (!frontend_timer) basesrv_idle_fatal("Create frontend timer",GetLastError());
        /* Arm only after listening succeeds, never race stop against startup. */
        result=RpcServerListen(1,RPC_C_LISTEN_MAX_CALLS_DEFAULT,TRUE);
    }
    if (!result) {
        /* Readiness is a successful client RPC, never this diagnostic line. */
        fwprintf(stdout,L"basesrv: listening on %ls\n",endpoint); fflush(stdout);
        /* Manual and launcher starts have the same ten-second empty grace. */
        basesrv_schedule_empty_stop();
        for(;;) {
            HANDLE waits[3]={idle_timer,frontend_timer,
                OpenNtBaseServiceFrontendLifetimeChanged(service)};
            ULONGLONG deadline=0,now;
            LARGE_INTEGER due;
            DWORD wait,error=OpenNtBaseServiceRetireExpiredFrontends(service);
            if(error)basesrv_idle_fatal("frontend deadline",error);
            error=OpenNtBaseServiceNextFrontendDeadline(service,&deadline);
            if(error)basesrv_idle_fatal("frontend admission deadline",error);
            if(deadline) {
                now=GetTickCount64();
                due.QuadPart=-(LONGLONG)(deadline>now ? deadline-now : 1u)*10000;
                if(!SetWaitableTimer(frontend_timer,&due,0,NULL,NULL,FALSE))
                    basesrv_idle_fatal("Set frontend timer",GetLastError());
            } else if(!CancelWaitableTimer(frontend_timer))
                basesrv_idle_fatal("Cancel frontend timer",GetLastError());
            wait=WaitForMultipleObjects(3,waits,FALSE,INFINITE);
            if(wait==WAIT_OBJECT_0) {
                if(basesrv_claim_empty_stop())break;
            } else if(wait==WAIT_OBJECT_0+1) {
                error=OpenNtBaseServiceRetireExpiredFrontends(service);
                if(error)basesrv_idle_fatal("frontend retirement",error);
            } else if(wait!=WAIT_OBJECT_0+2)
                basesrv_idle_fatal("lifetime wait",GetLastError());
        }
        result=RpcMgmtStopServerListening(NULL);
        if (result) basesrv_idle_fatal("RpcMgmtStopServerListening",result);
        result=RpcMgmtWaitServerListen();
        if (result) basesrv_idle_fatal("RpcMgmtWaitServerListen",result);
    }
    {
        RPC_STATUS cleanup=RpcServerUnregisterIf(Server_vdm_service_v31_0_s_ifspec,NULL,TRUE);
        if (!result && cleanup) result=cleanup;
    }
    if (idle_timer) CloseHandle(idle_timer);
    if (frontend_timer) CloseHandle(frontend_timer);
    if (!OpenNtBaseServiceStop(service)) return ERROR_BUSY;
    return (int)result;
}
