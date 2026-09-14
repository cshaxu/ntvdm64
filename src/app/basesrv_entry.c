/* Standalone BaseSrv composition. S3 integration in progress: the current
 * endpoint offers authenticated registration/first-VDM only, not DOS launch.
 * It is build-only until command/resource/worker and idle gates are complete. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include "service.h"
#include "broker/rpc_security.h"
#include "adapter-opennt-host/basesrv/include/base_service.h"
static broker_rpc_scope scope;
static OPENNT_BASE_SERVICE *service;
void *__RPC_USER midl_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER midl_user_free(void *value) { free(value); }
void __RPC_USER VDM_CONNECTION_rundown(VDM_CONNECTION connection)
{
    /* No commands/resources can be admitted by this endpoint yet. */
    if (OpenNtBaseServiceDisconnect(connection)) RaiseFailFastException(NULL,NULL,0);
}
static RPC_STATUS RPC_ENTRY authorize(RPC_IF_HANDLE interfaceId,void *binding)
{
    (void)interfaceId;
    return broker_rpc_authorize(&scope,binding);
}
error_status_t Server_Connect(handle_t binding,HANDLE process,VDM_CONNECTION *connection,ULONG *generation)
{
    DWORD pid;
    RPC_STATUS status;
    *connection=NULL; *generation=0;
    status=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (status) return status;
    return OpenNtBaseServiceConnect(service,process,(OPENNT_BASE_CONNECTION **)connection,generation);
}
error_status_t Server_First(handle_t binding,VDM_CONNECTION connection,HANDLE process,ULONG generation,ULONG *first)
{
    DWORD pid;
    RPC_STATUS status;
    *first=0;
    status=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (status) return status;
    return OpenNtBaseServiceFirst(connection,pid,generation,first);
}
error_status_t Server_Disconnect(handle_t binding,HANDLE process,ULONG generation,VDM_CONNECTION *connection)
{
    DWORD pid,result=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (result) return result;
    if (!OpenNtBaseServicePeer(*connection,pid,generation)) return ERROR_ACCESS_DENIED;
    result=OpenNtBaseServiceDisconnect(*connection);
    if (!result) *connection=NULL;
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
    result=RpcServerRegisterIf3(Server_vdm_service_v1_0_s_ifspec,NULL,NULL,
        RPC_IF_ALLOW_SECURE_ONLY | RPC_IF_ALLOW_LOCAL_ONLY,RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        (unsigned)-1,authorize,NULL);
    if (!result) {
        /* Readiness is a successful client RPC, never this diagnostic line. */
        fwprintf(stdout,L"basesrv: listening on %ls\n",endpoint); fflush(stdout);
        result=RpcServerListen(1,RPC_C_LISTEN_MAX_CALLS_DEFAULT,FALSE);
        RpcServerUnregisterIf(Server_vdm_service_v1_0_s_ifspec,NULL,TRUE);
    }
    if (!OpenNtBaseServiceStop(service)) return ERROR_BUSY;
    return (int)result;
}
