/* Smallest public-RPC replacement for the unavailable NT4 CSR port.  This
 * file owns no BaseSrv policy: it authenticates/binds one local process and
 * forwards only operations with an implemented copied endpoint. */
#include <windows.h>
#include <rpc.h>
#include <stdlib.h>
#include <stdio.h>
#include "service.h"
#include "ntsrv-exe/transport/rpc_security.h"
#include <base_rpc_client.h>
#include "common/protocol/version.h" /* Shared metadata, no product behavior. */
#include "common/protocol/frontend_lifecycle.h"
#include "common/console/members.h"
#include "common/rpc/local_binding.h"
#include "common/rpc/native_command.h"
#include "common/rpc/frontend_control.h"
#include "common/rpc/worker_control.h"
#include "common/rpc/dos_observation.h"
#include "common/rpc/async_call.h"
#include <base_rpc_client_internal.h>

typedef struct OPENNT_BASE_RPC_CLIENT {
    RPC_BINDING_HANDLE binding;
    VDM_CONNECTION connection;
    HANDLE process;
    HANDLE server;
    HANDLE stop;
    HANDLE watcher;
    void (*lost)(void *);
    void *lost_context;
    ULONG generation;
    volatile LONG local_failure;
    BOOL bounded_calls;
} OPENNT_BASE_RPC_CLIENT;

static OPENNT_BASE_RPC_CLIENT client;
/* Optional execution-side resource cleanup, absent in frontend consumers. */
static PVOID volatile execution_cleanup;

static common_rpc_connection client_rpc_view(void)
{
    common_rpc_connection view={client.binding,client.connection,client.process,client.generation};
    return view;
}

/* Borrowed for a synchronous launch-side call. No resource ownership moves. */
common_rpc_connection OpenNtBaseClientExecutionConnection(void (*cleanup)(void))
{
    (void)InterlockedCompareExchangePointer(&execution_cleanup,(PVOID)cleanup,NULL);
    return client_rpc_view();
}

DWORD OpenNtBaseClientObserveDosEvent(const common_dos_observation *fact,BOOL gap,HANDLE stop)
{
    common_rpc_connection view=client_rpc_view();
    return common_rpc_observe_dos_event(&view,fact,gap,stop);
}

/* Notify only confirmed death of the authenticated service process. Neither
 * process termination nor role-specific resource cleanup belongs in transport. */
static DWORD WINAPI broker_lifetime_watch(void *context)
{
    OPENNT_BASE_RPC_CLIENT *state=context;
    HANDLE waits[2]={state->stop,state->server};
    DWORD result=WaitForMultipleObjects(2,waits,FALSE,INFINITE);
    if (result==WAIT_OBJECT_0+1) {
        (void)OpenNtBaseClientFailure(ERROR_INVALID_HANDLE);
        state->lost(state->lost_context);
    }
    return result==WAIT_FAILED ? GetLastError() : ERROR_SUCCESS;
}

/* Generated client stubs own only their transient marshalling buffers. */
void *__RPC_USER MIDL_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER MIDL_user_free(void *value) { free(value); }

/* UNKNOWN_IF also occurs while a healthy-version endpoint drains. Query the
 * same authenticated local endpoint before calling it a version mismatch.
 * This never admits work or relaxes the subsequent Connect identity check. */
static DWORD classify_missing_interface(RPC_BINDING_HANDLE binding)
{
    RPC_IF_ID expected;
    RPC_IF_ID_VECTOR *interfaces=NULL;
    RPC_STATUS status,uuid_status;
    unsigned int index;
    DWORD result=RPC_S_SERVER_UNAVAILABLE;
    status=RpcIfInqId(Client_vdm_service_v51_0_c_ifspec,&expected);
    if (status) return status;
    status=RpcMgmtInqIfIds(binding,&interfaces);
    if (status) return status;
    for (index=0;index<interfaces->Count;++index) {
        RPC_IF_ID *peer=interfaces->IfId[index];
        if (UuidEqual(&peer->Uuid,&expected.Uuid,&uuid_status) &&
            (peer->VersMajor!=expected.VersMajor || peer->VersMinor!=expected.VersMinor)) {
            result=ERROR_REVISION_MISMATCH;
            break;
        }
    }
    RpcIfIdVectorFree(&interfaces);
    return result;
}

DWORD OpenNtBaseClientConnectCurrent(void)
{
    return OpenNtBaseClientConnectCurrentTimeout(INFINITE);
}

DWORD OpenNtBaseClientFailure(DWORD error)
{
    DWORD failure=(DWORD)InterlockedCompareExchange(&client.local_failure,0,0);
    if(failure)return failure;
    if(client.server && WaitForSingleObject(client.server,0)==WAIT_OBJECT_0)
        failure=ERROR_INVALID_HANDLE;
    else if(client.bounded_calls && (error==ERROR_TIMEOUT || error==RPC_S_CALL_CANCELLED))
        failure=ERROR_TIMEOUT;
    if(failure){
        DWORD previous=(DWORD)InterlockedCompareExchange(&client.local_failure,(LONG)failure,0);
        return previous ? previous : failure;
    }
    return error;
}

DWORD OpenNtBaseClientConnectCurrentTimeout(DWORD milliseconds)
{
    static const unsigned char expected[APP_VERSION_BYTES]=APP_VERSION;
    unsigned char server_version[APP_VERSION_BYTES]={0};
    ULONG server_protocol=0;
    broker_rpc_scope scope;
    WCHAR endpoint[128];
    DWORD error=ERROR_GEN_FAILURE;
    ULONG generation=0;
    VDM_CONNECTION connection=NULL;
    ULONGLONG deadline=milliseconds==INFINITE ? 0 : GetTickCount64()+milliseconds;
    RPC_ASYNC_STATE async;

    if(!milliseconds)return ERROR_TIMEOUT;
    if (client.connection) return ERROR_ALREADY_EXISTS;
    if (!broker_rpc_capture_scope(&scope)) return GetLastError();
    wsprintfW(endpoint,L"ntvdm-basesrv-%lu-%08lx-%08lx",scope.session,
        (ULONG)scope.logon.HighPart,scope.logon.LowPart);
    /* Preserve this source-shaped client's historical binding-failure result;
     * the neutral mechanism itself returns the precise RPC status. */
    if(common_rpc_bind_local(endpoint,&client.binding))goto done;
    client.bounded_calls=milliseconds!=INFINITE;
    /* Original BaseSrvUpdateDOSEntry duplicates the authenticated worker's
     * self pseudo-handle; no broader process access is needed. */
    client.process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE|PROCESS_DUP_HANDLE,
        FALSE,GetCurrentProcessId());
    if (!client.process) { error=GetLastError(); goto done; }
    error=common_rpc_begin_async(&async);if(error)goto done;
    RpcTryExcept {
        Client_Connect(&async,client.binding,client.process,APP_PROTOCOL_VERSION,
            (unsigned char *)expected,&server_protocol,server_version,&connection,&generation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,common_rpc_remaining_ms(deadline));
    if (error==RPC_S_UNKNOWN_IF) error=classify_missing_interface(client.binding);
    /* A confirmed legacy RPC major cannot decode this Connect signature. */
    if (error==RPC_S_PROCNUM_OUT_OF_RANGE ||
        (error==ERROR_REVISION_MISMATCH && !server_protocol)) {
        fprintf(stderr,"ntvdm client: version mismatch: local protocol=%u app=%s; broker RPC interface incompatible\n",
            APP_PROTOCOL_VERSION,APP_VERSION);
        error=ERROR_REVISION_MISMATCH;
    } else if (error==ERROR_REVISION_MISMATCH || (!error &&
        (server_protocol!=APP_PROTOCOL_VERSION || memcmp(server_version,expected,sizeof(expected))))) {
        fprintf(stderr,"ntvdm client: version mismatch: local protocol=%u app=%s; broker protocol=%lu app=%.32s\n",
            APP_PROTOCOL_VERSION,APP_VERSION,server_protocol,(const char *)server_version);
        error=ERROR_REVISION_MISMATCH;
    }
    /* Retain any returned context solely for cleanup on a rejected response;
     * it never becomes available to original BaseClient task operations. */
    if (connection) { client.connection=connection; client.generation=generation; }
    if (!error && (!connection || !generation)) error=ERROR_INVALID_DATA;
    if (error) goto done;
    if(!common_rpc_remaining_ms(deadline)){error=ERROR_TIMEOUT;goto done;}
    error=common_rpc_begin_async(&async);if(error)goto done;
    RpcTryExcept {
        Client_BrokerProcess(&async,client.binding,client.connection,client.process,
            client.generation,&client.server);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,common_rpc_remaining_ms(deadline));
    if (error) goto done;
    if (!client.server || WaitForSingleObject(client.server,0)!=WAIT_TIMEOUT) {
        error=RPC_S_SERVER_UNAVAILABLE; goto done;
    }
    error=ERROR_SUCCESS;
done:
    if (error) {
        error=OpenNtBaseClientFailure(error);
        /* A failed bootstrap has no admitted request to finalize. Context
         * rundown owns any partially accepted connection; do not spend a
         * second connect budget attempting a cleanup RPC. */
        client.local_failure=error;
        OpenNtBaseClientDisconnectCurrent();
    }
    return error;
}

DWORD OpenNtBaseClientReportCurrentConsoleMembers(void)
{
    DWORD count,error,*members=NULL;
    if (!client.connection || !client.process) return ERROR_INVALID_STATE;
    /* Only the attached root samples identity. Preserve its two-read ceiling;
     * common owns allocation, never authentication or registration policy. */
    error=common_console_members_read(32,4096,2,&members,&count);
    if(error) {
        if(error==ERROR_BUFFER_OVERFLOW)error=ERROR_ARITHMETIC_OVERFLOW;
        if(error==ERROR_GEN_FAILURE)error=ERROR_INVALID_HANDLE;
        goto done;
    }
    RpcTryExcept {
        error=Client_ReportConsoleMembers(client.binding,client.connection,client.process,
            client.generation,count,members);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
done:
    common_console_members_release(members);
    return error;
}

DWORD OpenNtBaseClientWatchBroker(void (*lost)(void *),void *context)
{
    if (!client.server || !lost) return ERROR_INVALID_STATE;
    if (client.watcher) return ERROR_ALREADY_EXISTS;
    client.stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if (!client.stop) return GetLastError();
    client.lost=lost;client.lost_context=context;
    client.watcher=CreateThread(NULL,0,broker_lifetime_watch,&client,0,NULL);
    if (!client.watcher) {
        DWORD error=GetLastError();
        CloseHandle(client.stop);client.stop=NULL;
        return error;
    }
    return ERROR_SUCCESS;
}

void OpenNtBaseClientStopBrokerWatch(void)
{
    if(client.watcher) {
        SetEvent(client.stop);
        WaitForSingleObject(client.watcher,INFINITE);
        CloseHandle(client.watcher);client.watcher=NULL;
    }
    if(client.stop){CloseHandle(client.stop);client.stop=NULL;}
    client.lost=NULL;client.lost_context=NULL;
}

void OpenNtBaseClientDisconnectCurrent(void)
{
    VDM_CONNECTION connection;
    void (*cleanup)(void);
    OpenNtBaseClientStopBrokerWatch();
    if (!client.local_failure && client.connection && client.binding && client.process &&
        (!client.server || WaitForSingleObject(client.server,0)==WAIT_TIMEOUT)) {
        connection=client.connection;
        RpcTryExcept {
            (void)Client_Disconnect(client.binding,client.process,
                client.generation,&connection);
        }
        RpcExcept(1) { }
        RpcEndExcept
    }
    if (client.process) CloseHandle(client.process);
    if (client.server) CloseHandle(client.server);
    if (client.binding) RpcBindingFree(&client.binding);
    cleanup=(void (*)(void))InterlockedExchangePointer(&execution_cleanup,NULL);
    if (cleanup) cleanup();
    ZeroMemory(&client,sizeof(client));
}

DWORD OpenNtBaseClientWorkerFrontendCapability(HANDLE *capability)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_worker_frontend_capability(&state,capability);
}

DWORD OpenNtBaseClientAcquireConsoleContext(HANDLE frontend,HANDLE *capability)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_acquire_console_context(&state,frontend,capability);
}
DWORD OpenNtBaseClientBindConsoleContext(HANDLE capability)
{
    common_rpc_connection state=client_rpc_view();
    return OpenNtBaseClientFailure(common_rpc_bind_console_context(&state,capability));
}

DWORD OpenNtBaseClientGetNextNativeCommand(DWORD capacity,BYTE *payload,DWORD *bytes,HANDLE *caller_process,HANDLE *execution,HANDLE *frontend,DWORD *request,DWORD *caller_generation)
{
    common_rpc_connection view=client_rpc_view();
    return common_rpc_next_native_command(&view,capacity,payload,bytes,caller_process,
        execution,frontend,request,caller_generation);
}

DWORD OpenNtBaseClientNativeStartupResult(DWORD caller_generation,DWORD request,DWORD status,HANDLE target,HANDLE receipt)
{
    common_rpc_connection view=client_rpc_view();
    return common_rpc_native_startup_result(&view,caller_generation,request,status,target,receipt);
}

DWORD OpenNtBaseClientRegisterNativeBackend(HANDLE frontend)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_register_native_backend(&state,frontend);
}
DWORD OpenNtBaseClientCompleteWorkerChannel(DWORD request,DWORD exit_code)
{
    return OpenNtBaseClientCompleteNativeRequest(request,exit_code,0,0);
}
DWORD OpenNtBaseClientCompleteNativeRequest(DWORD request,DWORD exit_code,DWORD io_error,DWORD io_flags)
{
    common_rpc_connection view=client_rpc_view();
    return common_rpc_complete_native_command(&view,request,exit_code,io_error,io_flags);
}
DWORD OpenNtBaseClientNativeExitCode(DWORD request,DWORD *exit_code)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!request || !exit_code)return ERROR_INVALID_PARAMETER;
    *exit_code=0;
    if(!client.connection || !client.binding || !client.process)return error;
    RpcTryExcept {
        error=Client_NativeExitCode(client.binding,client.connection,client.process,
            client.generation,request,exit_code);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    return error;
}
DWORD OpenNtBaseClientBindNativeTarget(DWORD request,HANDLE target,HANDLE receipt)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!request || !target || !receipt || !client.connection || !client.binding || !client.process)
        return error;
    RpcTryExcept {
        error=Client_BindNativeTarget(client.binding,client.connection,client.process,
            client.generation,request,target,receipt);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    return error;
}
DWORD OpenNtBaseClientRegisterFrontendRoot(HANDLE capability)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=Client_RegisterFrontendRoot(client.binding,client.connection,client.process,
            client.generation,capability);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return error;
}
DWORD OpenNtBaseClientAcquireFrontendRoot(uint64_t console_window,DWORD *create_root,
    HANDLE *root,HANDLE *capability,HANDLE *retire,HANDLE *restored)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_acquire_frontend_root(&state,console_window,create_root,root,capability,retire,restored);
}
DWORD OpenNtBaseClientCancelFrontendRootReservation(void)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!client.connection || !client.binding || !client.process)return error;
    RpcTryExcept {
        error=Client_CancelFrontendRootReservation(client.binding,client.connection,
            client.process,client.generation);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return error;
}
DWORD OpenNtBaseClientRegisterFrontendLease(uint64_t console_window,DWORD creator_pid,
    BOOL borrowed,HANDLE retire,HANDLE restored)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!client.connection || !client.binding || !client.process)return error;
    RpcTryExcept {
        error=Client_RegisterFrontendLease(client.binding,client.connection,client.process,
            client.generation,(LONGLONG)console_window,creator_pid,borrowed!=FALSE,retire,restored);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return error;
}
DWORD OpenNtBaseClientFrontendJoinCandidate(DWORD *nonce,DWORD *candidate_pid)
{
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE;
    if(!nonce || !candidate_pid)return ERROR_INVALID_PARAMETER;
    *nonce=*candidate_pid=0;
    if(!client.connection || !client.binding || !client.process)return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_FrontendJoinCandidate(&async,client.binding,client.connection,client.process,
            client.generation,nonce,candidate_pid);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,client.server,
        OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
    if(error)*nonce=*candidate_pid=0;
    return error;
}
DWORD OpenNtBaseClientFrontendJoinDecision(DWORD nonce,BOOL same_console)
{
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE;
    if(!client.connection || !client.binding || !client.process)return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_FrontendJoinDecision(&async,client.binding,client.connection,client.process,
            client.generation,nonce,same_console!=FALSE);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=common_rpc_end_async(&async,error,client.server,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
    return OpenNtBaseClientFailure(error);
}
DWORD OpenNtBaseClientFrontendLeaseReady(void)
{
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE;
    if(!client.connection || !client.binding || !client.process)return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_FrontendLeaseReady(&async,client.binding,client.connection,client.process,
            client.generation);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=common_rpc_end_async(&async,error,client.server,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
    return OpenNtBaseClientFailure(error);
}

DWORD OpenNtBaseClientFrontendUsage(DWORD *pending,DWORD *tasks)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_frontend_usage(&state,pending,tasks);
}

DWORD OpenNtBaseClientRetireWorkerlessFrontend(DWORD *retired)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_retire_workerless_frontend(&state,retired));
}

DWORD OpenNtBaseClientFrontendStateChanged(HANDLE *state_changed)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_frontend_state_changed(&state,state_changed);
}

DWORD OpenNtBaseClientBrokerProcess(HANDLE *server)
{
    if(!server)return ERROR_INVALID_PARAMETER;
    *server=NULL;
    if(OpenNtBaseClientFailure(0))return client.local_failure;
    if(!client.server)return ERROR_INVALID_STATE;
    return DuplicateHandle(GetCurrentProcess(),client.server,GetCurrentProcess(),
        server,PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0) ? 0 : GetLastError();
}
DWORD OpenNtBaseClientStartNativeWorker(DWORD native_type,HANDLE *worker)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!worker)return ERROR_INVALID_PARAMETER;
    *worker=NULL;
    if(!client.connection || !client.binding || !client.process)return error;
    RpcTryExcept {
        error=Client_StartNativeWorker(client.binding,client.connection,client.process,
            client.generation,native_type,worker);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if(error && *worker){CloseHandle(*worker);*worker=NULL;}
    return OpenNtBaseClientFailure(error);
}
DWORD OpenNtBaseClientSubmitNativeRequest(DWORD operation,HANDLE frontend,DWORD bytes,BYTE *payload,
    HANDLE *target,HANDLE *receipt,DWORD *request)
{
    common_rpc_connection view=client_rpc_view();
    return OpenNtBaseClientFailure(common_rpc_submit_native_request(&view,operation,frontend,bytes,payload,target,receipt,request));
}
DWORD OpenNtBaseClientFinishNativeRequest(DWORD request,DWORD *exit_code,DWORD *target_completed)
{
    common_rpc_connection view=client_rpc_view();
    return OpenNtBaseClientFailure(common_rpc_finish_native_request(&view,request,exit_code,target_completed));
}
DWORD OpenNtBaseClientStartFrontend(uint64_t window,BOOL borrowed,
    HANDLE *root,HANDLE *capability,HANDLE *restored)
{
    common_rpc_connection state=client_rpc_view();
    return OpenNtBaseClientFailure(common_rpc_start_frontend(&state,window,borrowed,root,capability,restored));
}
DWORD OpenNtBaseClientFrontendConsoleRestored(void)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_frontend_console_restored(&state));
}
DWORD OpenNtBaseClientFrontendCloseQuiesced(void)
{
    RPC_ASYNC_STATE async;
    DWORD error=OpenNtBaseClientFailure(0);
    if(error)return error;
    if(!client.binding || !client.connection || !client.process)return ERROR_INVALID_STATE;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_FrontendCloseQuiesced(&async,client.binding,client.connection,
            client.process,client.generation);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return OpenNtBaseClientFailure(common_rpc_end_async(&async,error,client.server,
        OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
}
DWORD OpenNtBaseClientFrontendStartupResult(HANDLE capability,DWORD status)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_frontend_startup_result(&state,capability,status);
}
DWORD OpenNtBaseClientWaitFrontendConsoleRestored(void)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error=OpenNtBaseClientFailure(0);
    if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_wait_frontend_console_restored(&state));
}

DWORD OpenNtBaseClientWorkerShutdownEvent(HANDLE *shutdown)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_worker_shutdown_event(&state,shutdown);
}

DWORD OpenNtBaseClientRequestWorkerExit(void)
{
    RPC_ASYNC_STATE async={0};
    DWORD error=OpenNtBaseClientFailure(0);
    if(error)return error;
    if(!client.binding || !client.connection || !client.process)return ERROR_INVALID_STATE;
    /* Local RPC ignores RPC_C_OPT_CALL_TIMEOUT. Reuse the existing async
     * completion/cancellation owner without changing concurrent GetNext calls. */
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_RequestWorkerExit(&async,client.binding,client.connection,
            client.process,client.generation);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=common_rpc_end_async(&async,error,client.server,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
    return OpenNtBaseClientFailure(error);
}

DWORD OpenNtBaseClientWorkerConsoleCloseEvent(HANDLE *close)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_worker_console_close_event(&state,close);
}

DWORD OpenNtBaseClientWorkerStateChanged(HANDLE *state_changed)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_worker_state_changed(&state,state_changed);
}

DWORD OpenNtBaseClientWorkerIoReleaseEvent(HANDLE *release)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_worker_io_release_event(&state,release);
}

DWORD OpenNtBaseClientWorkerIoTransition(DWORD action)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_worker_io_transition(&state,action);
}
DWORD OpenNtBaseClientWorkerIoCheckpoint(DWORD reason,DWORD request,DWORD *decision)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_worker_io_checkpoint(&state,reason,request,decision);
}
DWORD OpenNtBaseClientFrontendIoDisconnected(void)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_frontend_io_disconnected(&state));
}


DWORD OpenNtBaseClientRetainFrontendRoot(HANDLE capability,HANDLE *root,DWORD *generation)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!root || !generation) return ERROR_INVALID_PARAMETER;
    *root=NULL;*generation=0;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=Client_RetainFrontendRoot(client.binding,client.connection,client.process,
            client.generation,capability,root,generation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error) {
        if (*root) CloseHandle(*root);
        *root=NULL;*generation=0;
    }
    return OpenNtBaseClientFailure(error);
}

DWORD OpenNtBaseClientRequestFrontend(HANDLE capability)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=Client_RequestFrontend(client.binding,client.connection,client.process,
            client.generation,capability);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return OpenNtBaseClientFailure(error);
}

DWORD OpenNtBaseClientFrontendRequest(DWORD *request,HANDLE *worker)
{
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE;
    if (!request || !worker) return ERROR_INVALID_PARAMETER;
    *request=0;*worker=NULL;
    if (!client.connection || !client.binding || !client.process) return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_FrontendRequest(&async,client.binding,client.connection,client.process,
            client.generation,request,worker);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,client.server,
        OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
    if (error) {
        if (*worker) CloseHandle(*worker);
        *worker=NULL;*request=0;
    }
    return error;
}

DWORD OpenNtBaseClientAttachFrontendRequest(DWORD request,HANDLE pipe,HANDLE ready,DWORD *generation)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!generation) return ERROR_INVALID_PARAMETER;
    *generation=0;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=Client_AttachFrontendRequest(client.binding,client.connection,client.process,
            client.generation,request,pipe,ready);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (!error) *generation=client.generation;
    return error;
}

static DWORD client_command_worker(HANDLE *worker,BOOL select_native)
{
    DWORD error=ERROR_INVALID_HANDLE;
    if (!worker) return ERROR_INVALID_PARAMETER;
    *worker=NULL;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=select_native ? Client_SelectNativeWorker(client.binding,client.connection,client.process,
            client.generation,worker) : Client_CommandWorker(client.binding,client.connection,client.process,
            client.generation,worker);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error && *worker) { CloseHandle(*worker); *worker=NULL; }
    return error;
}

DWORD OpenNtBaseClientCommandWorker(HANDLE *worker)
{ return client_command_worker(worker,FALSE); }
DWORD OpenNtBaseClientSelectNativeWorker(HANDLE *worker)
{ return client_command_worker(worker,TRUE); }

DWORD OpenNtBaseClientAttachFrontend(HANDLE pipe,DWORD *generation,HANDLE ready)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!generation) return ERROR_INVALID_PARAMETER;
    *generation=0;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=Client_AttachFrontend(client.binding,client.connection,client.process,
            client.generation,pipe,ready);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (!error) *generation=client.generation;
    return error;
}

static DWORD take_frontend(HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready,BOOL wait)
{
    common_rpc_connection state=client_rpc_view();
    return common_rpc_take_frontend(&state,pipe,frontend,frontend_generation,ready,wait);
}

DWORD OpenNtBaseClientTakeFrontend(HANDLE *pipe,HANDLE *frontend,DWORD *generation,HANDLE *ready)
{ return take_frontend(pipe,frontend,generation,ready,FALSE); }
DWORD OpenNtBaseClientWaitFrontend(HANDLE *pipe,HANDLE *frontend,DWORD *generation,HANDLE *ready)
{ return take_frontend(pipe,frontend,generation,ready,TRUE); }

static DWORD reserve_worker(ULONG task,BOOL native_worker,uint64_t *reservation)
{
    hyper id=0;
    DWORD error=ERROR_INVALID_STATE;
    if (!reservation) return ERROR_INVALID_PARAMETER;
    *reservation=0;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=Client_Reserve(client.binding,client.connection,client.process,
            client.generation,task,native_worker ? 1u : 0u,&id);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (!error && id>0) *reservation=(uint64_t)id;
    else if (!error) error=ERROR_INVALID_DATA;
    return error;
}

DWORD OpenNtBaseClientReserveWorker(ULONG task,uint64_t *reservation)
{ return reserve_worker(task,FALSE,reservation); }

DWORD OpenNtBaseClientReserveNativeWorker(uint64_t *reservation)
{ return reserve_worker(0,TRUE,reservation); }

DWORD OpenNtBaseClientPrepareWorker(uint64_t reservation,HANDLE worker)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!reservation || !worker || worker==INVALID_HANDLE_VALUE) return ERROR_INVALID_PARAMETER;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=Client_Prepare(client.binding,client.connection,client.process,
            client.generation,(hyper)reservation,worker);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return error;
}

DWORD OpenNtBaseClientReleaseWorker(uint64_t reservation)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!reservation) return ERROR_INVALID_PARAMETER;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=Client_Release(client.binding,client.connection,client.process,
            client.generation,(hyper)reservation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return error;
}
