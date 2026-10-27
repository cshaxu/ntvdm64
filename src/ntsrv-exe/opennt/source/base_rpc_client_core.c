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
#include "common/protocol/frontend_protocol.h"
#include "common/console/members.h"
#include "common/rpc/local_binding.h"
#include "common/rpc/native_command.h"
#include "common/rpc/frontend_control.h"
#include "common/rpc/worker_control.h"
#include "common/rpc/dos_observation.h"
#include "common/rpc/async_call.h"
#include "common/rpc/interface_query.h"
#include <base_rpc_client_internal.h>

typedef struct OPENNT_BASE_RPC_CLIENT {
    RPC_BINDING_HANDLE binding;
    VDM_CONNECTION connection;
    HANDLE process;
    HANDLE server;
    HANDLE worker_shutdown,worker_console_close,worker_wow_task_close;
    HANDLE stop;
    HANDLE watcher;
    void (*lost)(void *);
    void *lost_context;
    ULONG generation;
    volatile LONG local_failure;
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
static DWORD classify_missing_interface(RPC_BINDING_HANDLE binding,ULONGLONG deadline)
{
    RPC_IF_ID expected;
    RPC_STATUS status=RpcIfInqId(Client_vdm_service_v65_0_c_ifspec,&expected);
    if (status) return status;
    return common_rpc_classify_interface(binding,&expected,deadline);
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
    wsprintfW(endpoint,BROKER_RPC_ENDPOINT_FORMAT,scope.session,
        (ULONG)scope.logon.HighPart,scope.logon.LowPart,scope.package_id);
    /* Preserve this source-shaped client's historical binding-failure result;
     * the neutral mechanism itself returns the precise RPC status. */
    if(common_rpc_bind_local(endpoint,&client.binding))goto done;
    /* Original BaseSrvUpdateDOSEntry duplicates the authenticated worker's
     * self pseudo-handle; no broader process access is needed. */
    client.process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE|PROCESS_DUP_HANDLE,
        FALSE,GetCurrentProcessId());
    if (!client.process) { error=GetLastError(); goto done; }
    error=common_rpc_begin_async(&async);if(error)goto done;
    RpcTryExcept {
        Client_Connect(&async,client.binding,client.process,APP_PROTOCOL_VERSION,
            (unsigned char *)expected,&server_protocol,server_version,&connection,&generation,
            &client.server,&client.worker_shutdown,&client.worker_console_close,&client.worker_wow_task_close);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,common_rpc_remaining_ms(deadline));
    if (error==RPC_S_UNKNOWN_IF) error=classify_missing_interface(client.binding,deadline);
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
    RPC_ASYNC_STATE async;
    DWORD count,error,*members=NULL;
    if (!client.connection || !client.process) return ERROR_INVALID_STATE;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    /* Only the attached root samples identity. Preserve its two-read ceiling;
     * common owns allocation, never authentication or registration policy. */
    error=common_console_members_read(32,4096,2,&members,&count);
    if(error) {
        if(error==ERROR_BUFFER_OVERFLOW)error=ERROR_ARITHMETIC_OVERFLOW;
        if(error==ERROR_GEN_FAILURE)error=ERROR_INVALID_HANDLE;
        goto done;
    }
    error=common_rpc_begin_async(&async);if(error)goto done;
    RpcTryExcept {
        Client_ReportConsoleMembers(&async,client.binding,client.connection,client.process,
            client.generation,count,members);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,client.server,
        INFINITE));
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
        RPC_ASYNC_STATE async;
        DWORD error=common_rpc_begin_async(&async);
        connection=client.connection;
        if(!error) {
            RpcTryExcept {
                Client_Disconnect(&async,client.binding,client.process,
                    client.generation,&connection);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            /* Cleanup never replaces the caller's established task result.
             * Drain cancellation before releasing context/binding storage. */
            (void)common_rpc_end_async(&async,error,client.server,
                INFINITE);
        }
    }
    if (client.process) CloseHandle(client.process);
    if (client.server) CloseHandle(client.server);
    if (client.worker_shutdown) CloseHandle(client.worker_shutdown);
    if (client.worker_console_close) CloseHandle(client.worker_console_close);
    if (client.worker_wow_task_close) CloseHandle(client.worker_wow_task_close);
    if (client.binding) RpcBindingFree(&client.binding);
    cleanup=(void (*)(void))InterlockedExchangePointer(&execution_cleanup,NULL);
    if (cleanup) cleanup();
    ZeroMemory(&client,sizeof(client));
}

static DWORD client_worker_event(DWORD (*query)(const common_rpc_connection *,HANDLE *),HANDLE *output)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error;
    if(!output)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    error=OpenNtBaseClientFailure(0);
    if(!error)error=OpenNtBaseClientFailure(query(&state,output));
    if(error && *output){CloseHandle(*output);*output=NULL;}
    return error;
}
DWORD OpenNtBaseClientWorkerFrontendCapability(HANDLE *capability)
{ return client_worker_event(common_rpc_worker_frontend_capability,capability); }

DWORD OpenNtBaseClientAcquireConsoleContext(HANDLE frontend,HANDLE *capability)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error;
    if(!capability)return ERROR_INVALID_PARAMETER;
    *capability=NULL;
    error=OpenNtBaseClientFailure(0);
    if(!error)error=OpenNtBaseClientFailure(common_rpc_acquire_console_context(&state,frontend,capability));
    if(error && *capability){CloseHandle(*capability);*capability=NULL;}
    return error;
}
static DWORD client_worker_handle(DWORD (*query)(const common_rpc_connection *,HANDLE),HANDLE handle)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(query(&state,handle));
}
DWORD OpenNtBaseClientBindConsoleContext(HANDLE capability)
{ return client_worker_handle(common_rpc_bind_console_context,capability); }

DWORD OpenNtBaseClientGetNextNativeCommand(DWORD capacity,BYTE *payload,DWORD *bytes,HANDLE *caller_process,HANDLE *execution,HANDLE *frontend,DWORD *request,DWORD *caller_generation)
{
    common_rpc_connection view=client_rpc_view();
    return common_rpc_next_native_command(&view,capacity,payload,bytes,caller_process,
        execution,frontend,request,caller_generation);
}

DWORD OpenNtBaseClientNativeStartupResult(DWORD caller_generation,DWORD request,DWORD status,HANDLE target,HANDLE receipt)
{
    common_rpc_connection view=client_rpc_view();
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_native_startup_result(&view,caller_generation,request,status,target,receipt));
}

DWORD OpenNtBaseClientRegisterNativeBackend(HANDLE frontend)
{ return client_worker_handle(common_rpc_register_native_backend,frontend); }
DWORD OpenNtBaseClientCompleteWorkerChannel(DWORD request,DWORD exit_code)
{
    return OpenNtBaseClientCompleteNativeRequest(request,exit_code,0,0);
}
DWORD OpenNtBaseClientCompleteNativeRequest(DWORD request,DWORD exit_code,DWORD io_error,DWORD io_flags)
{
    common_rpc_connection view=client_rpc_view();
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_complete_native_command(&view,request,exit_code,io_error,io_flags));
}
DWORD OpenNtBaseClientBindNativeTarget(DWORD request,HANDLE target,HANDLE receipt)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if(!request || !target || !receipt || !client.connection || !client.binding || !client.process)
        return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_BindNativeTarget(&async,client.binding,client.connection,client.process,
            client.generation,request,target,receipt);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    return OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,INFINITE));
}
DWORD OpenNtBaseClientRegisterFrontendRoot(HANDLE capability)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if (!client.connection || !client.binding || !client.process) return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_RegisterFrontendRoot(&async,client.binding,client.connection,client.process,
            client.generation,capability);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return OpenNtBaseClientFailure(common_rpc_end_async(&async,error,client.server,
        INFINITE));
}
DWORD OpenNtBaseClientRegisterFrontendLease(uint64_t console_window,DWORD creator_pid,
    BOOL borrowed,HANDLE retire,HANDLE restored)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if(!client.connection || !client.binding || !client.process)return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_RegisterFrontendLease(&async,client.binding,client.connection,client.process,
            client.generation,(LONGLONG)console_window,creator_pid,borrowed!=FALSE,retire,restored);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return OpenNtBaseClientFailure(common_rpc_end_async(&async,error,client.server,
        INFINITE));
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
        INFINITE));
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
    error=common_rpc_end_async(&async,error,client.server,INFINITE);
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
    error=common_rpc_end_async(&async,error,client.server,INFINITE);
    return OpenNtBaseClientFailure(error);
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
    DWORD error;
    if(!state_changed)return ERROR_INVALID_PARAMETER;
    *state_changed=NULL;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=OpenNtBaseClientFailure(common_rpc_frontend_state_changed(&state,state_changed));
    if(error && *state_changed){CloseHandle(*state_changed);*state_changed=NULL;}
    return error;
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
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if(!worker)return ERROR_INVALID_PARAMETER;
    *worker=NULL;
    if(!client.connection || !client.binding || !client.process)return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_StartNativeWorker(&async,client.binding,client.connection,client.process,
            client.generation,native_type,worker);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,INFINITE));
    if(error && *worker){CloseHandle(*worker);*worker=NULL;}
    return error;
}
DWORD OpenNtBaseClientSubmitNativeRequest(DWORD operation,HANDLE frontend,DWORD bytes,BYTE *payload,
    HANDLE *target,HANDLE *receipt,DWORD *request)
{
    common_rpc_connection view=client_rpc_view();
    DWORD error;
    if(!target || !receipt || !request)return ERROR_INVALID_PARAMETER;
    *target=*receipt=NULL;*request=0;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=OpenNtBaseClientFailure(common_rpc_submit_native_request(&view,operation,frontend,bytes,payload,target,receipt,request));
    if(error){if(*target)CloseHandle(*target);if(*receipt)CloseHandle(*receipt);
        *target=*receipt=NULL;*request=0;}
    return error;
}
DWORD OpenNtBaseClientFinishNativeRequest(DWORD request,DWORD *exit_code,DWORD *target_completed)
{
    common_rpc_connection view=client_rpc_view();
    DWORD error;
    if(!exit_code || !target_completed)return ERROR_INVALID_PARAMETER;
    *exit_code=*target_completed=0;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_finish_native_request(&view,request,exit_code,target_completed));
}
DWORD OpenNtBaseClientStartFrontend(uint64_t window,BOOL borrowed,
    HANDLE *root,HANDLE *capability,HANDLE *restored)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error;
    if(!root || !capability || !restored)return ERROR_INVALID_PARAMETER;
    *root=*capability=*restored=NULL;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=OpenNtBaseClientFailure(common_rpc_start_frontend(&state,window,borrowed,root,capability,restored));
    /* A successful wire reply can race confirmed service loss. Do not leak
     * its attachments or publish success after the final failure decision. */
    if(error){
        if(*root)CloseHandle(*root);if(*capability)CloseHandle(*capability);
        if(*restored)CloseHandle(*restored);
        *root=*capability=*restored=NULL;
    }
    return error;
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
        INFINITE));
}
DWORD OpenNtBaseClientFrontendStartupResult(HANDLE capability,DWORD status)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_frontend_startup_result(&state,capability,status));
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
    if(!shutdown)return ERROR_INVALID_PARAMETER;
    *shutdown=NULL;
    if(!client.worker_shutdown)return ERROR_ACCESS_DENIED;
    return DuplicateHandle(GetCurrentProcess(),client.worker_shutdown,GetCurrentProcess(),
        shutdown,SYNCHRONIZE,FALSE,0) ? ERROR_SUCCESS : GetLastError();
}

DWORD OpenNtBaseClientWorkerControls(HANDLE *server,HANDLE *shutdown,HANDLE *console_close,HANDLE *wow_task_close)
{
    if(!server || !shutdown || !console_close || !wow_task_close)return ERROR_INVALID_PARAMETER;
    *server=NULL;*shutdown=NULL;*console_close=NULL;*wow_task_close=NULL;
    if(!client.connection || !client.server || !client.worker_shutdown)return ERROR_INVALID_STATE;
    *server=client.server;*shutdown=client.worker_shutdown;*console_close=client.worker_console_close;
    *wow_task_close=client.worker_wow_task_close;
    return ERROR_SUCCESS;
}

DWORD WINAPI OpenNtBaseClientBindWowTask(ULONG task,USHORT htask,uint64_t *identity)
{
    RPC_ASYNC_STATE async;DWORD error;hyper value=0;
    /* Shared WOW supplies its original iTask; a separate WOW is authenticated
     * by its consumed one-shot reservation and deliberately supplies zero. */
    if(!identity || !htask)return ERROR_INVALID_PARAMETER;*identity=0;
    if(!client.connection || !client.binding || !client.process)return ERROR_ACCESS_DENIED;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {Client_BindWowTask(&async,client.binding,client.connection,client.process,client.generation,task,htask,&value);}
    RpcExcept(1){error=RpcExceptionCode();} RpcEndExcept
    error=common_rpc_end_async(&async,error,client.server,INFINITE);
    if(!error)*identity=(uint64_t)value;return OpenNtBaseClientFailure(error);
}
DWORD WINAPI OpenNtBaseClientRetireWowTask(uint64_t identity)
{
    RPC_ASYNC_STATE async;DWORD error;
    if(!identity)return ERROR_INVALID_PARAMETER;
    if(!client.connection || !client.binding || !client.process)return ERROR_ACCESS_DENIED;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {Client_RetireWowTask(&async,client.binding,client.connection,client.process,client.generation,(hyper)identity);}
    RpcExcept(1){error=RpcExceptionCode();} RpcEndExcept
    return OpenNtBaseClientFailure(common_rpc_end_async(&async,error,client.server,INFINITE));
}
DWORD OpenNtBaseClientTakeWowTaskClose(uint64_t *identity)
{
    RPC_ASYNC_STATE async;DWORD error;hyper value=0;
    if(!identity)return ERROR_INVALID_PARAMETER;*identity=0;
    if(!client.connection || !client.binding || !client.process)return ERROR_ACCESS_DENIED;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {Client_TakeWowTaskClose(&async,client.binding,client.connection,client.process,client.generation,&value);}
    RpcExcept(1){error=RpcExceptionCode();} RpcEndExcept
    error=common_rpc_end_async(&async,error,client.server,INFINITE);
    if(!error)*identity=(uint64_t)value;return OpenNtBaseClientFailure(error);
}
DWORD WINAPI OpenNtBaseClientFailWowTaskClose(uint64_t identity)
{
    RPC_ASYNC_STATE async;DWORD error;
    if(!identity)return ERROR_INVALID_PARAMETER;
    if(!client.connection || !client.binding || !client.process)return ERROR_ACCESS_DENIED;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {Client_FailWowTaskClose(&async,client.binding,client.connection,client.process,client.generation,(hyper)identity);}
    RpcExcept(1){error=RpcExceptionCode();} RpcEndExcept
    return OpenNtBaseClientFailure(common_rpc_end_async(&async,error,client.server,INFINITE));
}

static DWORD client_request_exit(DWORD scope)
{
    RPC_ASYNC_STATE async={0};
    DWORD error=OpenNtBaseClientFailure(0);
    if(error)return error;
    if(!client.binding || !client.connection || !client.process)return ERROR_INVALID_STATE;
    /* Local RPC ignores RPC_C_OPT_CALL_TIMEOUT. Reuse the existing async
     * completion/cancellation owner without changing concurrent GetNext calls. */
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_RequestParticipantExit(&async,client.binding,client.connection,
            client.process,client.generation,scope);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=common_rpc_end_async(&async,error,client.server,INFINITE);
    return OpenNtBaseClientFailure(error);
}

DWORD OpenNtBaseClientRequestParticipantExit(void)
{ return client_request_exit(OPENNT_EXIT_PARTICIPANT); }

DWORD OpenNtBaseClientRequestConsoleExit(void)
{ return client_request_exit(OPENNT_EXIT_CONSOLE); }

DWORD OpenNtBaseClientWorkerConsoleCloseEvent(HANDLE *close)
{
    if(!close)return ERROR_INVALID_PARAMETER;
    *close=NULL;
    if(!client.worker_console_close)return client.worker_shutdown ? ERROR_NOT_SUPPORTED : ERROR_ACCESS_DENIED;
    return DuplicateHandle(GetCurrentProcess(),client.worker_console_close,GetCurrentProcess(),
        close,SYNCHRONIZE,FALSE,0) ? ERROR_SUCCESS : GetLastError();
}

DWORD OpenNtBaseClientWorkerIoReleaseEvent(HANDLE *release)
{ return client_worker_event(common_rpc_worker_io_release_event,release); }

DWORD OpenNtBaseClientWorkerIoTransition(DWORD action)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_worker_io_transition(&state,action));
}
DWORD OpenNtBaseClientWorkerIoCheckpoint(DWORD reason,DWORD request,DWORD *decision)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error;
    if(!decision)return ERROR_INVALID_PARAMETER;
    *decision=WORKER_IO_KEEP;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=OpenNtBaseClientFailure(common_rpc_worker_io_checkpoint(&state,reason,request,decision));
    if(error)*decision=WORKER_IO_KEEP;
    return error;
}
DWORD OpenNtBaseClientFrontendIoDisconnected(void)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_frontend_io_disconnected(&state));
}


DWORD OpenNtBaseClientRetainFrontendRoot(HANDLE capability,HANDLE *root,DWORD *generation)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if (!root || !generation) return ERROR_INVALID_PARAMETER;
    *root=NULL;*generation=0;
    if (!client.connection || !client.binding || !client.process) return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_RetainFrontendRoot(&async,client.binding,client.connection,client.process,
            client.generation,capability,root,generation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,INFINITE));
    if (error) {
        if (*root) CloseHandle(*root);
        *root=NULL;*generation=0;
    }
    return error;
}

DWORD OpenNtBaseClientRequestFrontend(HANDLE capability)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if (!client.connection || !client.binding || !client.process) return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_RequestFrontend(&async,client.binding,client.connection,client.process,
            client.generation,capability);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,INFINITE));
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
        INFINITE));
    if (error) {
        if (*worker) CloseHandle(*worker);
        *worker=NULL;*request=0;
    }
    return error;
}

DWORD OpenNtBaseClientAttachFrontendRequest(DWORD request,HANDLE pipe,HANDLE ready,DWORD *generation)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if (!generation) return ERROR_INVALID_PARAMETER;
    *generation=0;
    if (!client.connection || !client.binding || !client.process) return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_AttachFrontendRequest(&async,client.binding,client.connection,client.process,
            client.generation,request,pipe,ready);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,client.server,
        INFINITE));
    if (!error) *generation=client.generation;
    return error;
}


static DWORD take_frontend(HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready,BOOL wait)
{
    common_rpc_connection state=client_rpc_view();
    DWORD error;
    if(!pipe || !frontend || !frontend_generation || !ready)return ERROR_INVALID_PARAMETER;
    *pipe=*frontend=*ready=NULL;*frontend_generation=0;
    error=OpenNtBaseClientFailure(0);
    if(!error)error=OpenNtBaseClientFailure(common_rpc_take_frontend(&state,pipe,frontend,frontend_generation,ready,wait));
    if(error) {
        if(*pipe)CloseHandle(*pipe);
        if(*frontend)CloseHandle(*frontend);
        if(*ready)CloseHandle(*ready);
        *pipe=*frontend=*ready=NULL;*frontend_generation=0;
    }
    return error;
}

DWORD OpenNtBaseClientTakeFrontend(HANDLE *pipe,HANDLE *frontend,DWORD *generation,HANDLE *ready)
{ return take_frontend(pipe,frontend,generation,ready,FALSE); }
DWORD OpenNtBaseClientWaitFrontend(HANDLE *pipe,HANDLE *frontend,DWORD *generation,HANDLE *ready)
{ return take_frontend(pipe,frontend,generation,ready,TRUE); }
