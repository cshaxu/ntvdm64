/* Standalone BaseSrv composition. S3 integration in progress: the current
 * endpoint offers registration/first-VDM and typed stream receipts, not DOS launch.
 * It is build-only until command/resource/worker and idle gates are complete. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include "service.h"
#include "basesrv-exe/transport/rpc_security.h"
#include "opennt-abi/source/public/internal/base/inc/vdmapi.h"
#include "basesrv-exe/opennt/include/base_service.h"
#include "basesrv-exe/console_query.h"
#include "product-abi/version.h"
static broker_rpc_scope scope;
static OPENNT_BASE_SERVICE *service;
static WCHAR console_helper[MAX_PATH];
static SRWLOCK idle_lock=SRWLOCK_INIT;
static HANDLE idle_timer;
static ULONG idle_epoch;
#define BASESRV_EMPTY_GRACE_MS 60000u
#define BASE_CHECK_REPLY_BYTES 40u
#define BASE_UPDATE_REPLY_BYTES 32u

static BOOL basesrv_sibling_path(PCWSTR name,PWSTR output,DWORD capacity)
{
    DWORD length;
    PWSTR slash;
    if (!name || !output || !capacity) return FALSE;
    length=GetModuleFileNameW(NULL,output,capacity);
    if (!length || length>=capacity || !(slash=wcsrchr(output,L'\\'))) return FALSE;
    ++slash;
    if ((DWORD)(slash-output)+lstrlenW(name)+1>capacity) return FALSE;
    lstrcpyW(slash,name);
    return TRUE;
}
static DWORD WINAPI basesrv_console_query(void *context,HANDLE caller,
    const HANDLE *candidates,DWORD count,HANDLE cancel,DWORD timeout,BYTE *members)
{
    return app_console_query((const WCHAR *)context,caller,candidates,count,cancel,timeout,members);
}

/* Default-off S3 transport observation.  This intentionally records neither
 * copied command bytes nor any OS handle/pointer: it exists only to identify
 * the original service operation and its externally observable result. */
static void basesrv_trace(const char *phase,DWORD pid,DWORD status)
{
    CHAR path[MAX_PATH],line[128];
    DWORD length,written,saved=GetLastError();
    HANDLE file;
    int bytes;
    length=GetEnvironmentVariableA("MVDM_BASESRV_TRACE_PATH",path,sizeof(path));
    if (!length || length>=sizeof(path)) goto done;
    bytes=wsprintfA(line,"BASESRV-S3 phase=%s pid=%lu status=%08lX\r\n",
        phase,(unsigned long)pid,(unsigned long)status);
    if (bytes<=0 || (size_t)bytes>=sizeof(line)) goto done;
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ,NULL,OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL);
    if (file!=INVALID_HANDLE_VALUE) {
        (void)WriteFile(file,line,(DWORD)bytes,&written,NULL);
        CloseHandle(file);
    }
done:
    SetLastError(saved);
}
/* This is product retention, not BaseSrv task policy.  An interactive
 * COMMAND/EDIT worker remains registered, so it never reaches this path just
 * because its command queue has no new entry.  The one-minute grace starts
 * only after every authenticated connection and finite launch reservation
 * has already gone away. */
static VOID CALLBACK basesrv_empty_timer(PVOID context,BOOLEAN fired)
{
    ULONG epoch=(ULONG)(ULONG_PTR)context;
    (void)fired;
    AcquireSRWLockExclusive(&idle_lock);
    /* Cancellation and every replacement advance the epoch before releasing
     * the lock.  A stale timer may run, but can never stop a broker that a
     * new authenticated Connect has kept alive. */
    if (epoch!=idle_epoch || !idle_timer) {
        ReleaseSRWLockExclusive(&idle_lock);
        return;
    }
    idle_timer=NULL;
    if (service && OpenNtBaseServiceIsEmpty(service)) {
        basesrv_trace("empty-stop",0,ERROR_SUCCESS);
        (void)RpcMgmtStopServerListening(NULL);
    }
    ReleaseSRWLockExclusive(&idle_lock);
}
static void basesrv_cancel_empty_timer(void)
{
    HANDLE timer;
    AcquireSRWLockExclusive(&idle_lock);
    timer=idle_timer;idle_timer=NULL;++idle_epoch;
    ReleaseSRWLockExclusive(&idle_lock);
    /* Do not wait for a callback while holding idle_lock: the callback takes
     * the same lock to test its epoch. */
    if (timer) (void)DeleteTimerQueueTimer(NULL,timer,INVALID_HANDLE_VALUE);
}
static void basesrv_schedule_empty_stop(void)
{
    AcquireSRWLockExclusive(&idle_lock);
    if (!idle_timer && service && OpenNtBaseServiceIsEmpty(service)) {
        if (!++idle_epoch) ++idle_epoch;
        if (CreateTimerQueueTimer(&idle_timer,NULL,basesrv_empty_timer,
                (PVOID)(ULONG_PTR)idle_epoch,BASESRV_EMPTY_GRACE_MS,0,WT_EXECUTEDEFAULT))
            basesrv_trace("empty-grace",0,ERROR_SUCCESS);
    }
    ReleaseSRWLockExclusive(&idle_lock);
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
    unsigned char application_version[32],hyper *epoch,ULONG *count,DTASKMGR_WORKER **entries)
{
    OPENNT_BASE_WORKER_INFO *local=NULL;
    DWORD pid,error;
    uint32_t actual=0,index;
    uint64_t service_epoch=0;
    if (!epoch || !count || !entries) return ERROR_INVALID_PARAMETER;
    *epoch=0; *count=0; *entries=NULL;
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
            (*entries)[index].sequence=local[index].sequence;
            (*entries)[index].kind=local[index].kind;
            (*entries)[index].state=local[index].state;
            (*entries)[index].reserved=local[index].reserved;
            (*entries)[index].started_filetime=(hyper)local[index].started_filetime;
            (*entries)[index].task=local[index].task;
            (*entries)[index].stack_depth=local[index].stack_depth;
            memcpy((*entries)[index].image,local[index].image,sizeof(local[index].image));
        }
    }
    *epoch=(hyper)service_epoch;
    *count=actual;
done:
    if (local) HeapFree(GetProcessHeap(),0,local);
    if (error && *entries) { MIDL_user_free(*entries); *entries=NULL; }
    if (error) { *epoch=0; *count=0; }
    basesrv_schedule_empty_stop();
    basesrv_trace("task-snapshot",pid,error);
    return error;
}
error_status_t Server_TerminateWorker(handle_t binding,HANDLE process,ULONG protocol,
    unsigned char application_version[32],hyper epoch,ULONG sequence)
{
    DWORD pid,error;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=basesrv_management_version(protocol,application_version);
    if (!error) error=OpenNtBaseServiceTerminateWorker(service,(uint64_t)epoch,sequence);
    basesrv_schedule_empty_stop();
    basesrv_trace("task-terminate",pid,error);
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
    RPC_STATUS status;
    (void)interfaceId;
    status=broker_rpc_authorize(&scope,binding);
    /* An RPC call that has passed endpoint authentication is already an
     * accepted arrival, even before Server_Connect can register its context.
     * Cancel here so an empty-grace callback cannot stop listening between
     * RPC acceptance and the later connection registration. */
    if (!status) basesrv_cancel_empty_timer();
    return status;
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
            basesrv_trace("export-handle",GetCurrentProcessId(),error);
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
    status=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (status) { basesrv_trace("connect-auth",0,status); return status; }
    if (protocol!=APP_PROTOCOL_VERSION || memcmp(application_version,expected,sizeof(expected))) {
        fprintf(stderr,"basesrv: version mismatch: local protocol=%u app=%s; peer protocol=%lu app=%.32s\n",
            APP_PROTOCOL_VERSION,APP_VERSION,protocol,(const char *)application_version);
        basesrv_trace("connect-version",pid,ERROR_REVISION_MISMATCH);
        basesrv_schedule_empty_stop();
        return ERROR_REVISION_MISMATCH;
    }
    /* Cancel before registering the new peer.  The callback and this
     * registration are serialized by idle_lock, so an arrival cannot be
     * mistaken for an empty broker between its health check and stop call. */
    basesrv_cancel_empty_timer();
    status=OpenNtBaseServiceConnect(service,process,(OPENNT_BASE_CONNECTION **)connection,generation);
    if (status) basesrv_schedule_empty_stop();
    basesrv_trace("connect",pid,status);
    return status;
}
error_status_t Server_BrokerProcess(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,HANDLE *server)
{
    DWORD pid,error;
    *server=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_ACCESS_DENIED;
    /* RPC consumes this non-inheritable, wait-only duplicate. The receiver
     * cannot terminate or modify the server through this capability. */
    if (!DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
            server,SYNCHRONIZE,FALSE,0)) return GetLastError();
    return ERROR_SUCCESS;
}
error_status_t Server_First(handle_t binding,VDM_CONNECTION connection,HANDLE process,ULONG generation,ULONG *first)
{
    DWORD pid;
    RPC_STATUS status;
    *first=0;
    status=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (status) return status;
    status=OpenNtBaseServiceFirst(connection,pid,generation,first);
    /* Default-off observation of the original BaseSrvIsFirstVDM result.  The
     * broker owns no substitute first-worker state. */
    basesrv_trace(status ? "first-error" : (*first ? "first-yes" : "first-no"),pid,status);
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
    if (error) basesrv_trace("check-service",pid,error);
    if (!error && required!=BASE_CHECK_REPLY_BYTES) return ERROR_INVALID_DATA;
    if (!error && parent_event) {
        error=export_handles(&parent_event,1,parentEvents);
        if (error) return error;
        *parentEventCount=1;
    }
    if (!error) *parentReceipt=(ULONG)parent_receipt;
    if (!error) *replyBytes=required;
    basesrv_trace("check",pid,error);
    return error;
}
error_status_t Server_Get(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG requestBytes,unsigned char *request,ULONG *waitEventCount,HANDLE **waitEvents,
    ULONG *streamCount,HANDLE **streams,ULONG *replyBytes,
    unsigned char **reply)
{
    DWORD pid,error;
    uint32_t bytes=0;
    ULONG standard_count=0;
    void *source_reply=NULL;
    HANDLE wait_event=NULL;
    HANDLE standard[3]={NULL,NULL,NULL};
    if (!waitEventCount || !waitEvents || !streamCount || !streams || !replyBytes || !reply) return ERROR_INVALID_PARAMETER;
    *waitEventCount=0; *waitEvents=NULL; *streamCount=0; *streams=NULL; *replyBytes=0; *reply=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceGet(connection,pid,generation,request,requestBytes,
        &source_reply,&bytes,&wait_event,standard,&standard_count);
    if (error) {
        basesrv_trace("get",pid,error);
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
    error=export_handles(standard,standard_count,streams);
    if (error) {
        MIDL_user_free(*reply); *reply=NULL;
        if (*waitEvents) { CloseHandle(**waitEvents); MIDL_user_free(*waitEvents);
            *waitEvents=NULL; *waitEventCount=0; }
        OpenNtBaseServiceReleaseCommandReply(source_reply); return error;
    }
    *streamCount=standard_count;
    OpenNtBaseServiceReleaseCommandReply(source_reply);
    *replyBytes=bytes;
    basesrv_trace("get",pid,ERROR_SUCCESS);
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
    basesrv_trace("exit",pid,error);
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
    basesrv_trace("exit-code",pid,error);
    return error;
}
error_status_t Server_Reenter(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG increment)
{
    DWORD pid,error;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceReenter(connection,pid,generation,increment);
    basesrv_trace(increment==INCREMENT_REENTER_COUNT ? "reenter-inc" : "reenter-dec",
        pid,error);
    return error;
}
error_status_t Server_Reserve(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG task,hyper *reservation)
{
    DWORD pid,error;
    uint64_t id=0;
    if (!reservation) return ERROR_INVALID_PARAMETER;
    *reservation=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceCreateReservation(connection,pid,generation,task,&id);
    if (!error) *reservation=(hyper)id;
    basesrv_trace("reserve",pid,error);
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
    basesrv_trace("prepare",pid,error);
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
    basesrv_trace("disconnect",pid,result);
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
    if (!basesrv_sibling_path(L"run16.exe",console_helper,MAX_PATH) ||
        !OpenNtBaseServiceConfigureConsoleQuery(service,basesrv_console_query,console_helper) ||
        !OpenNtBaseServiceConfigureEmptyNotify(service,basesrv_worker_terminated,NULL)) {
        DWORD error=GetLastError();
        (void)OpenNtBaseServiceStop(service);
        return (int)error;
    }
    result=RpcServerRegisterIf3(Server_vdm_service_v3_0_s_ifspec,NULL,NULL,
        RPC_IF_ALLOW_SECURE_ONLY | RPC_IF_ALLOW_LOCAL_ONLY,RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        (unsigned)-1,authorize,NULL);
    if (!result) {
        /* Readiness is a successful client RPC, never this diagnostic line. */
        fwprintf(stdout,L"basesrv: listening on %ls\n",endpoint); fflush(stdout);
        /* A manually started broker with no client/worker is also empty.
         * It receives the same one-minute grace as a broker drained after a
         * normal worker/launcher teardown. */
        basesrv_schedule_empty_stop();
        result=RpcServerListen(1,RPC_C_LISTEN_MAX_CALLS_DEFAULT,FALSE);
        RpcServerUnregisterIf(Server_vdm_service_v3_0_s_ifspec,NULL,TRUE);
    }
    basesrv_cancel_empty_timer();
    if (!OpenNtBaseServiceStop(service)) return ERROR_BUSY;
    return (int)result;
}
