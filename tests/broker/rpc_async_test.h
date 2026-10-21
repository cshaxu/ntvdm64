#ifndef TEST_RPC_ASYNC_H
#define TEST_RPC_ASYNC_H
#include "service.h"
#include "common/rpc/async_call.h"
/* Raw-wire fixtures vary authentication/version inputs intentionally. Reuse
 * production cancellation, never a test transport or replacement endpoint. */
static __inline DWORD test_rpc_connect(handle_t binding,HANDLE process,ULONG protocol,
    unsigned char *application,ULONG *server_protocol,unsigned char *server_version,
    VDM_CONNECTION *connection,ULONG *generation)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {
        Client_Connect(&async,binding,process,protocol,application,server_protocol,
            server_version,connection,generation);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_broker_process(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,HANDLE *server)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_BrokerProcess(&async,binding,connection,process,generation,server);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_disconnect(handle_t binding,HANDLE process,ULONG generation,
    VDM_CONNECTION *connection)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_Disconnect(&async,binding,process,generation,connection);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_register_frontend_root(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,HANDLE capability)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_RegisterFrontendRoot(&async,binding,connection,process,generation,capability);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
#endif
