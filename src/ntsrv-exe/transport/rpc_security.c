/* Modern local RPC authentication only. Original BaseSrv remains the VDM
 * policy owner; NT4 CSR port/process internals are not recreated here. */
#include "rpc_security.h"
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

static BOOL token_scope(HANDLE token, broker_rpc_scope *scope)
{
    TOKEN_STATISTICS stats;
    DWORD bytes;
    if (!GetTokenInformation(token,TokenStatistics,&stats,sizeof(stats),&bytes) ||
        !GetTokenInformation(token,TokenSessionId,&scope->session,sizeof(scope->session),&bytes)) return FALSE;
    scope->logon=stats.AuthenticationId;
    scope->valid=TRUE;
    return TRUE;
}

BOOL broker_rpc_capture_scope(broker_rpc_scope *scope)
{
    HANDLE token;
    BOOL result;
    DWORD error;
    if (!scope) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    ZeroMemory(scope,sizeof(*scope));
    if (!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&token)) return FALSE;
    result=token_scope(token,scope);
    error=GetLastError();
    CloseHandle(token);
    SetLastError(error);
    return result;
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
        token_scope(token,&peer) && peer.session==scope->session &&
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
