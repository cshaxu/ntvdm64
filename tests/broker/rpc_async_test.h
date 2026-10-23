#ifndef TEST_RPC_ASYNC_H
#define TEST_RPC_ASYNC_H
#include "service.h"
#include "common/rpc/async_call.h"
/* Raw-wire fixtures vary authentication/version inputs intentionally. Reuse
 * production cancellation, never a test transport or replacement endpoint. */
static __inline DWORD test_rpc_connect_controls(handle_t binding,HANDLE process,ULONG protocol,
    unsigned char *application,ULONG *server_protocol,unsigned char *server_version,
    VDM_CONNECTION *connection,ULONG *generation,HANDLE *server,HANDLE *shutdown,HANDLE *console_close,
    HANDLE *wow_task_close)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {
        Client_Connect(&async,binding,process,protocol,application,server_protocol,
            server_version,connection,generation,server,shutdown,console_close,wow_task_close);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_connect(handle_t binding,HANDLE process,ULONG protocol,
    unsigned char *application,ULONG *server_protocol,unsigned char *server_version,
    VDM_CONNECTION *connection,ULONG *generation)
{
    HANDLE server=NULL,shutdown=NULL,console_close=NULL,wow_task_close=NULL;
    DWORD error=test_rpc_connect_controls(binding,process,protocol,application,
        server_protocol,server_version,connection,generation,&server,&shutdown,&console_close,&wow_task_close);
    if(server)CloseHandle(server);
    if(shutdown)CloseHandle(shutdown);
    if(console_close)CloseHandle(console_close);
    if(wow_task_close)CloseHandle(wow_task_close);
    return error;
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
static __inline DWORD test_rpc_acquire_console_context(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,HANDLE frontend,HANDLE *capability)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_AcquireConsoleContext(&async,binding,connection,process,generation,frontend,capability);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_bind_console_context(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,HANDLE capability)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_BindConsoleContext(&async,binding,connection,process,generation,capability);}
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
static __inline DWORD test_rpc_native_startup_result(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,ULONG caller_generation,ULONG request,ULONG status,HANDLE target,HANDLE receipt)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_NativeStartupResult(&async,binding,connection,process,generation,
        caller_generation,request,status,target,receipt);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_submit_native_request(handle_t binding,VDM_CONNECTION connection,
    HANDLE process,ULONG generation,ULONG operation,HANDLE frontend,ULONG bytes,BYTE *payload,
    HANDLE *target,HANDLE *receipt,ULONG *request)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_SubmitNativeRequest(&async,binding,connection,process,generation,
        operation,frontend,bytes,payload,target,receipt,request);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_first(handle_t binding, VDM_CONNECTION connection, HANDLE process, unsigned long generation, unsigned long *first)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_First(&async,binding,connection,process,generation,first);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_attach_pipe(handle_t binding, VDM_CONNECTION connection, HANDLE process, unsigned long generation, unsigned long role, HANDLE stream, unsigned long *receipt)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_AttachPipe(&async,binding,connection,process,generation,role,stream,receipt);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_attach_file(handle_t binding, VDM_CONNECTION connection, HANDLE process, unsigned long generation, unsigned long role, HANDLE stream, unsigned long *receipt)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_AttachFile(&async,binding,connection,process,generation,role,stream,receipt);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_revoke_stream(handle_t binding, VDM_CONNECTION connection, HANDLE process, unsigned long generation, unsigned long receipt)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_RevokeStream(&async,binding,connection,process,generation,receipt);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_task_snapshot(handle_t binding, HANDLE process, unsigned long protocol, byte application_version[ 32 ], unsigned long *count, DTASKMGR_WORKER **entries)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_TaskSnapshot(&async,binding,process,protocol,application_version,count,entries);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_worker_task_trace(handle_t binding, HANDLE process, unsigned long protocol, byte application_version[ 32 ], DTASKMGR_KEY *key, unsigned long *coverage, unsigned long *count, WORKER_TRACE_NODE **entries)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_WorkerTaskTrace(&async,binding,process,protocol,application_version,key,coverage,count,entries);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_wow_started(handle_t binding, VDM_CONNECTION connection, HANDLE process, unsigned long generation, unsigned long task)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_WowStarted(&async,binding,connection,process,generation,task);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
static __inline DWORD test_rpc_wow_startup(handle_t binding, VDM_CONNECTION connection, HANDLE process, unsigned long generation, unsigned long parent_receipt, HANDLE *event, unsigned long *started)
{
    RPC_ASYNC_STATE async;DWORD error=common_rpc_begin_async(&async);
    if(error)return error;
    RpcTryExcept {Client_WowStartup(&async,binding,connection,process,generation,parent_receipt,event,started);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,10000);
}
#endif
