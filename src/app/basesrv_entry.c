/* Standalone BaseSrv composition. S3 integration in progress: the current
 * endpoint offers registration/first-VDM and typed stream receipts, not DOS launch.
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
#define BASE_CHECK_REPLY_BYTES 40u
#define BASE_UPDATE_REPLY_BYTES 32u
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
void *__RPC_USER midl_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER midl_user_free(void *value) { free(value); }
void __RPC_USER VDM_CONNECTION_rundown(VDM_CONNECTION connection)
{
    /* No tasks yet; disconnect also drains retained input stream receipts. */
    if (OpenNtBaseServiceDisconnect(connection)) RaiseFailFastException(NULL,NULL,0);
    fputs("basesrv: connection rundown completed\n",stderr); fflush(stderr);
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
error_status_t Server_Check(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG requestBytes,unsigned char *request,ULONG *replyBytes,
    unsigned char reply[BASE_CHECK_REPLY_BYTES])
{
    DWORD pid,error;
    uint32_t required=0;
    if (!replyBytes || !reply) return ERROR_INVALID_PARAMETER;
    *replyBytes=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceCheck(connection,pid,generation,request,requestBytes,
        reply,BASE_CHECK_REPLY_BYTES,&required);
    if (!error && required!=BASE_CHECK_REPLY_BYTES) return ERROR_INVALID_DATA;
    if (!error) *replyBytes=required;
    return error;
}
error_status_t Server_Get(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG requestBytes,unsigned char *request,ULONG *waitEventCount,HANDLE **waitEvents,ULONG *replyBytes,
    unsigned char **reply)
{
    DWORD pid,error;
    uint32_t bytes=0;
    void *source_reply=NULL;
    HANDLE wait_event=NULL;
    if (!waitEventCount || !waitEvents || !replyBytes || !reply) return ERROR_INVALID_PARAMETER;
    *waitEventCount=0; *waitEvents=NULL; *replyBytes=0; *reply=NULL;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceGet(connection,pid,generation,request,requestBytes,
        &source_reply,&bytes,&wait_event);
    if (error) {
        fprintf(stderr,"basesrv: Get rejected %lu\n",error); fflush(stderr);
        return error;
    }
    if (wait_event) {
        if (!(*waitEvents=MIDL_user_allocate(sizeof(**waitEvents)))) {
            OpenNtBaseServiceReleaseCommandReply(source_reply);
            return ERROR_NOT_ENOUGH_MEMORY;
        }
        **waitEvents=wait_event;
        *waitEventCount=1;
    }
    if (!bytes || !(*reply=MIDL_user_allocate(bytes))) {
        if (*waitEvents) MIDL_user_free(*waitEvents);
        OpenNtBaseServiceReleaseCommandReply(source_reply);
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    memcpy(*reply,source_reply,bytes);
    OpenNtBaseServiceReleaseCommandReply(source_reply);
    *replyBytes=bytes;
    return ERROR_SUCCESS;
}
error_status_t Server_Update(handle_t binding,VDM_CONNECTION connection,HANDLE process,
    ULONG generation,ULONG requestBytes,unsigned char *request,ULONG *parentEventCount,HANDLE **parentEvents,
    ULONG *replyBytes,unsigned char reply[BASE_UPDATE_REPLY_BYTES])
{
    DWORD pid,error;
    uint32_t required=0;
    HANDLE parent_event=NULL;
    if (!parentEventCount || !parentEvents || !replyBytes || !reply) return ERROR_INVALID_PARAMETER;
    *parentEventCount=0; *parentEvents=NULL; *replyBytes=0;
    error=broker_rpc_peer_process(&scope,binding,process,&pid);
    if (error) return error;
    error=OpenNtBaseServiceUpdate(connection,pid,generation,request,requestBytes,reply,
        BASE_UPDATE_REPLY_BYTES,&required,&parent_event);
    if (error) {
        fprintf(stderr,"basesrv: Update rejected %lu\n",error); fflush(stderr);
        return error;
    }
    if (required!=BASE_UPDATE_REPLY_BYTES) return ERROR_INVALID_DATA;
    if (parent_event) {
        if (!(*parentEvents=MIDL_user_allocate(sizeof(**parentEvents)))) return ERROR_NOT_ENOUGH_MEMORY;
        **parentEvents=parent_event;
        *parentEventCount=1;
    }
    *replyBytes=required;
    return ERROR_SUCCESS;
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
