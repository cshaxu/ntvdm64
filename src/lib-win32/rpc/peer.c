#include "peer.h"
#include <rpcasync.h>

RPC_STATUS win32_rpc_local_peer_process(RPC_BINDING_HANDLE binding, HANDLE process, DWORD *pid)
{
    RPC_CALL_ATTRIBUTES_V2_W call={0};
    DWORD actual;
    RPC_STATUS status;
    if (!pid) return RPC_S_INVALID_ARG;
    *pid=0;
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
