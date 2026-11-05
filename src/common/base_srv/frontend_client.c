/* Frontend-role facade. Connection lifetime stays in the process core;
 * instance selection is explicit and presentation stays with the adapter. */
#include "frontend_client.h"
#include "base_rpc_client.h"
#include "base_rpc_client_internal.h"
#include "common/rpc/frontend_control.h"
#include "lib-win32/rpc/async_call.h"

DWORD OpenNtBaseClientFrontendIoDisconnected(const common_frontend_key *key)
{
    common_rpc_connection client=OpenNtBaseClientControlConnection(NULL);
    common_rpc_connection state=client;
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_frontend_io_disconnected(&state,key));
}

DWORD OpenNtBaseClientFrontendConsoleRestored(const common_frontend_key *key)
{
    common_rpc_connection client=OpenNtBaseClientControlConnection(NULL);
    common_rpc_connection state=client;
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_frontend_console_restored(&state,key));
}

DWORD OpenNtBaseClientFrontendCloseQuiesced(const common_frontend_key *key)
{
    HANDLE server=NULL;
    common_rpc_connection client=OpenNtBaseClientControlConnection(&server);
    RPC_ASYNC_STATE async;
    DWORD error=OpenNtBaseClientFailure(0);
    if(error)return error;
    if(!client.binding || !client.connection || !client.process)return ERROR_INVALID_STATE;
    error=common_rpc_begin_async(&async);if(error)return error;
    if(!key || !key->instance || !key->generation)return ERROR_INVALID_PARAMETER;
    RpcTryExcept {
        Client_FrontendCloseQuiesced(&async,client.binding,client.connection,
            client.process,client.generation,(LONGLONG)key->instance,key->generation);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return OpenNtBaseClientFailure(common_rpc_end_async(&async,error,server,
        INFINITE));
}

DWORD OpenNtBaseClientFrontendStartupResult(HANDLE capability,DWORD status)
{
    common_rpc_connection client=OpenNtBaseClientControlConnection(NULL);
    common_rpc_connection state=client;
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_frontend_startup_result(&state,capability,status));
}

DWORD OpenNtBaseClientRetireWorkerlessFrontend(const common_frontend_key *key,DWORD *retired)
{
    common_rpc_connection client=OpenNtBaseClientControlConnection(NULL);
    common_rpc_connection state=client;
    DWORD error=OpenNtBaseClientFailure(0);if(error)return error;
    return OpenNtBaseClientFailure(common_rpc_retire_workerless_frontend(&state,key,retired));
}

DWORD OpenNtBaseClientFrontendStateChanged(const common_frontend_key *key,HANDLE *state_changed)
{
    common_rpc_connection client=OpenNtBaseClientControlConnection(NULL);
    common_rpc_connection state=client;
    DWORD error;
    if(!state_changed)return ERROR_INVALID_PARAMETER;
    *state_changed=NULL;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=OpenNtBaseClientFailure(common_rpc_frontend_state_changed(&state,key,state_changed));
    if(error && *state_changed){CloseHandle(*state_changed);*state_changed=NULL;}
    return error;
}

DWORD OpenNtBaseClientRegisterFrontendRoot(HANDLE capability,common_frontend_key *key)
{
    HANDLE server=NULL;
    common_rpc_connection client=OpenNtBaseClientControlConnection(&server);
    RPC_ASYNC_STATE async;
    LONGLONG instance=0;ULONG frontend_generation=0;
    if(!key)return ERROR_INVALID_PARAMETER;
    ZeroMemory(key,sizeof(*key));
    DWORD error=ERROR_INVALID_STATE;
    if (!client.connection || !client.binding || !client.process) return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_RegisterFrontendRoot(&async,client.binding,client.connection,client.process,
            client.generation,capability,&instance,&frontend_generation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,server,INFINITE));
    if(!error){key->instance=(uint64_t)instance;key->generation=frontend_generation;}
    return error;
}

DWORD OpenNtBaseClientRegisterFrontendLease(const common_frontend_key *key,uint64_t console_window,DWORD creator_pid,
    BOOL borrowed,HANDLE retire,HANDLE restored)
{
    HANDLE server=NULL;
    common_rpc_connection client=OpenNtBaseClientControlConnection(&server);
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if(!client.connection || !client.binding || !client.process)return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    if(!key || !key->instance || !key->generation)return ERROR_INVALID_PARAMETER;
    RpcTryExcept {
        Client_RegisterFrontendLease(&async,client.binding,client.connection,client.process,
            client.generation,(LONGLONG)key->instance,key->generation,(LONGLONG)console_window,creator_pid,borrowed!=FALSE,retire,restored);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return OpenNtBaseClientFailure(common_rpc_end_async(&async,error,server,
        INFINITE));
}

DWORD OpenNtBaseClientFrontendJoinCandidate(const common_frontend_key *key,DWORD *nonce,DWORD *candidate_pid)
{
    HANDLE server=NULL;
    common_rpc_connection client=OpenNtBaseClientControlConnection(&server);
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE;
    if(!nonce || !candidate_pid)return ERROR_INVALID_PARAMETER;
    *nonce=*candidate_pid=0;
    if(!client.connection || !client.binding || !client.process)return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    if(!key || !key->instance || !key->generation)return ERROR_INVALID_PARAMETER;
    RpcTryExcept {
        Client_FrontendJoinCandidate(&async,client.binding,client.connection,client.process,
            client.generation,(LONGLONG)key->instance,key->generation,nonce,candidate_pid);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,server,
        INFINITE));
    if(error)*nonce=*candidate_pid=0;
    return error;
}

DWORD OpenNtBaseClientFrontendJoinDecision(const common_frontend_key *key,DWORD nonce,BOOL same_console)
{
    HANDLE server=NULL;
    common_rpc_connection client=OpenNtBaseClientControlConnection(&server);
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE;
    if(!client.connection || !client.binding || !client.process)return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    if(!key || !key->instance || !key->generation)return ERROR_INVALID_PARAMETER;
    RpcTryExcept {
        Client_FrontendJoinDecision(&async,client.binding,client.connection,client.process,
            client.generation,(LONGLONG)key->instance,key->generation,nonce,same_console!=FALSE);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=common_rpc_end_async(&async,error,server,INFINITE);
    return OpenNtBaseClientFailure(error);
}

DWORD OpenNtBaseClientFrontendLeaseReady(const common_frontend_key *key)
{
    HANDLE server=NULL;
    common_rpc_connection client=OpenNtBaseClientControlConnection(&server);
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE;
    if(!client.connection || !client.binding || !client.process)return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    if(!key || !key->instance || !key->generation)return ERROR_INVALID_PARAMETER;
    RpcTryExcept {
        Client_FrontendLeaseReady(&async,client.binding,client.connection,client.process,
            client.generation,(LONGLONG)key->instance,key->generation);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=common_rpc_end_async(&async,error,server,INFINITE);
    return OpenNtBaseClientFailure(error);
}

DWORD OpenNtBaseClientFrontendRequest(const common_frontend_key *key,DWORD *request,HANDLE *worker)
{
    HANDLE server=NULL;
    common_rpc_connection client=OpenNtBaseClientControlConnection(&server);
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE;
    if (!request || !worker) return ERROR_INVALID_PARAMETER;
    *request=0;*worker=NULL;
    if (!client.connection || !client.binding || !client.process) return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    if(!key || !key->instance || !key->generation)return ERROR_INVALID_PARAMETER;
    RpcTryExcept {
        Client_FrontendRequest(&async,client.binding,client.connection,client.process,
            client.generation,(LONGLONG)key->instance,key->generation,request,worker);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,server,
        INFINITE));
    if (error) {
        if (*worker) CloseHandle(*worker);
        *worker=NULL;*request=0;
    }
    return error;
}

DWORD OpenNtBaseClientAttachFrontendRequest(const common_frontend_key *key,DWORD request,HANDLE pipe,HANDLE ready,DWORD *generation)
{
    HANDLE server=NULL;
    common_rpc_connection client=OpenNtBaseClientControlConnection(&server);
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if (!generation) return ERROR_INVALID_PARAMETER;
    *generation=0;
    if (!client.connection || !client.binding || !client.process) return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    if(!key || !key->instance || !key->generation)return ERROR_INVALID_PARAMETER;
    RpcTryExcept {
        Client_AttachFrontendRequest(&async,client.binding,client.connection,client.process,
            client.generation,(LONGLONG)key->instance,key->generation,request,pipe,ready);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,server,
        INFINITE));
    if (!error) *generation=key->generation;
    return error;
}

DWORD OpenNtBaseClientRegisterEmbeddedFrontend(const common_frontend_location *location,
    common_frontend_key *key,HANDLE *notification,HANDLE *retire)
{
    HANDLE server=NULL;
    common_rpc_connection client=OpenNtBaseClientControlConnection(&server);
    RPC_ASYNC_STATE async;DWORD error;
    LONGLONG instance=0;ULONG generation=0;
    if(!location || !key || !notification || !retire)return ERROR_INVALID_PARAMETER;
    ZeroMemory(key,sizeof(*key));*notification=*retire=NULL;
    if(!client.connection || !client.binding || !client.process)return ERROR_INVALID_STATE;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_RegisterEmbeddedFrontend(&async,client.binding,client.connection,client.process,
            client.generation,(LONGLONG)location->payload,(LONGLONG)location->vm,
            (LONGLONG)location->incarnation,&instance,&generation,notification,retire);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,server,INFINITE));
    if(!error){key->instance=(uint64_t)instance;key->generation=generation;}
    else {
        if(*notification)CloseHandle(*notification);if(*retire)CloseHandle(*retire);
        *notification=*retire=NULL;
    }
    return error;
}

DWORD OpenNtBaseClientCloseFrontendInstance(const common_frontend_key *key)
{
    HANDLE server=NULL;
    common_rpc_connection client=OpenNtBaseClientControlConnection(&server);
    RPC_ASYNC_STATE async;DWORD error;
    if(!key || !key->instance || !key->generation)return ERROR_INVALID_PARAMETER;
    if(!client.connection || !client.binding || !client.process)return ERROR_INVALID_STATE;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_CloseFrontendInstance(&async,client.binding,client.connection,client.process,
            client.generation,(LONGLONG)key->instance,key->generation);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    return OpenNtBaseClientFailure(common_rpc_end_async(&async,error,server,INFINITE));
}

DWORD OpenNtBaseClientUnregisterFrontendInstance(const common_frontend_key *key)
{
    HANDLE server=NULL;
    common_rpc_connection client=OpenNtBaseClientControlConnection(&server);
    RPC_ASYNC_STATE async;DWORD error;
    if(!key || !key->instance || !key->generation)return ERROR_INVALID_PARAMETER;
    if(!client.connection || !client.binding || !client.process)return ERROR_INVALID_STATE;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_UnregisterFrontendInstance(&async,client.binding,client.connection,client.process,
            client.generation,(LONGLONG)key->instance,key->generation);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    return OpenNtBaseClientFailure(common_rpc_end_async(&async,error,server,INFINITE));
}
