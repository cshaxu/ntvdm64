/* Service-side RPC peer authorization remains NTSRV-owned. */
#include "rpc_security.h"
#include "common/base_srv/scope_internal.h"
#include <rpcasync.h>

RPC_STATUS broker_rpc_peer_process(const broker_rpc_scope *scope,
    RPC_BINDING_HANDLE binding, HANDLE process, DWORD *pid)
{
    RPC_CALL_ATTRIBUTES_V2_W call={0};
    DWORD actual;
    RPC_STATUS status;
    if (!pid) return RPC_S_INVALID_ARG;
    *pid=0;
    status=broker_rpc_authorize(scope,binding);
    if (status!=RPC_S_OK) return status;
    call.Version=2;
    call.Flags=RPC_QUERY_CLIENT_PID | RPC_QUERY_IS_CLIENT_LOCAL;
    status=RpcServerInqCallAttributesW(binding,&call);
    if (status!=RPC_S_OK) return status;
    actual=GetProcessId(process);
    if (call.IsClientLocal!=rcclLocal || !actual ||
        (ULONG_PTR)call.ClientPID!=actual || WaitForSingleObject(process,0)!=WAIT_TIMEOUT)
        return RPC_S_ACCESS_DENIED;
    *pid=actual;
    return RPC_S_OK;
}

RPC_STATUS broker_rpc_authorize(const broker_rpc_scope *scope, RPC_BINDING_HANDLE binding)
{
    HANDLE token=NULL;
    broker_rpc_scope peer={0};
    ULONG level=0, service=0;
    RPC_STATUS result=RPC_S_ACCESS_DENIED;
    if (!scope || !scope->valid) return result;
    if (RpcBindingInqAuthClientW(binding,NULL,NULL,&level,&service,NULL)!=RPC_S_OK ||
        level!=RPC_C_AUTHN_LEVEL_PKT_PRIVACY || service!=RPC_C_AUTHN_WINNT) return result;
    if (RpcImpersonateClient(binding)!=RPC_S_OK) return result;
    if (OpenThreadToken(GetCurrentThread(),TOKEN_QUERY,TRUE,&token) &&
        broker_rpc_token_scope(token,&peer) && peer.session==scope->session &&
        peer.logon.LowPart==scope->logon.LowPart && peer.logon.HighPart==scope->logon.HighPart)
        result=RPC_S_OK;
    if (token) CloseHandle(token);
    if (RpcRevertToSelf()!=RPC_S_OK) {
        /* Never dispatch or return a pooled thread still impersonating a peer. */
        if (!RevertToSelf()) RaiseFailFastException(NULL,NULL,0);
        return RPC_S_ACCESS_DENIED;
    }
    return result;
}
