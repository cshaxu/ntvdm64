/* Service-side RPC peer authorization remains NTSRV-owned. */
#include "rpc_security.h"
#include "common/base_srv/scope_internal.h"
#include "lib-win32/rpc/peer.h"

RPC_STATUS broker_rpc_peer_process(const broker_rpc_scope *scope,
    RPC_BINDING_HANDLE binding, HANDLE process, DWORD *pid)
{
    RPC_STATUS status;
    if (!pid) return RPC_S_INVALID_ARG;
    *pid=0;
    status=broker_rpc_authorize(scope,binding);
    if (status!=RPC_S_OK) return status;
    return win32_rpc_local_peer_process(binding,process,pid);
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
