/* Launch/execution extension of the NTSRV-owned RPC client.
 * NTCON does not link this translation unit. Connection and shutdown remain
 * core-owned; these calls borrow the same authenticated connection. */
#include <windows.h>
#include <rpc.h>
#include "service.h"
#include <nt.h>
#include <ntlpcapi.h>
#include <ntcsrmsg.h>
#include "non-mvdm/base/win32/inc/basemsg.h"
#include "non-mvdm/base/win32/inc/basevdm.h"
#include <vdmapi.h>
#include "compat/include/base_client.h"
#include "compat/include/base_transport.h"
#include "base_command.h"
#include "common/base_srv/base_rpc_client.h"
#include "common/base_srv/base_rpc_client_internal.h"
#include "common/base_srv/base_rpc_client_launch.h"
#include "lib-win32/rpc/async_call.h"
#include "common/protocol/frontend_lifecycle.h"
#include "compat/include/receipt_roles.h"

static LONG request_id;
static HANDLE parent_event_handle;
static ULONG parent_receipt;

static void reset_launch(void)
{
    ntvdm_base_bind_transport(NULL);
    parent_event_handle=NULL;
    parent_receipt=0;
}

uint64_t OpenNtBaseClientObservationDirect(void)
{
    (void)OpenNtBaseClientExecutionConnection(reset_launch);
    /* Read only by the same original guest execution thread as GetNext. */
    return ntvdm_base_observation_direct();
}

void OpenNtBaseClientSetCommandBinding(opennt_base_console_activate activate,
    struct compat_console_activation *activation)
{
    (void)OpenNtBaseClientExecutionConnection(reset_launch);
    ntvdm_base_set_activation(activate,activation);
}

static BOOL receipt_seen(const ULONG receipts[3],ULONG limit,ULONG receipt)
{
    ULONG index;
    for (index=0;index<limit;++index) if (receipts[index]==receipt) return TRUE;
    return FALSE;
}

/* Roll back each distinct attachment once; terminal transport failure leaves
 * the authenticated connection rundown responsible for service resources. */
static void revoke_streams(const common_rpc_connection *client,const ULONG receipts[3])
{
    ULONG index;
    for(index=0;!OpenNtBaseClientFailure(0) && index<3;++index) {
        RPC_ASYNC_STATE async;DWORD error;
        if(!receipts[index] || receipt_seen(receipts,index,receipts[index]))continue;
        error=common_rpc_begin_async(&async);if(error)break;
        RpcTryExcept {Client_RevokeStream(&async,client->binding,client->connection,
            client->process,client->generation,receipts[index]);}
        RpcExcept(1){error=RpcExceptionCode();}
        RpcEndExcept
        (void)OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
            INFINITE));
    }
}

/* Recognize only the caller's exact standard Console handle, not an
 * unrelated same-type file/pipe; preserve the original stream contract. */
static BOOL current_standard_console(HANDLE value,DWORD standard_id)
{
    HANDLE current=GetStdHandle(standard_id);
    DWORD mode;

    return value!=NULL && value==current &&
        GetConsoleMode(current,&mode);
}

static NTSTATUS rpc_failure(DWORD error)
{
    error=OpenNtBaseClientFailure(error);
    SetLastError(error);
    if(OpenNtBaseClientFailure(0)==ERROR_INVALID_HANDLE)return STATUS_PORT_DISCONNECTED;
    return STATUS_UNSUCCESSFUL;
}

/* BaseCheckVDM remains the classifier/record publisher. Its standard fields
 * cross RPC as typed attachments, preserving stdout/stderr aliasing. */
static DWORD attach_standard_streams(PBASE_API_MSG message,ULONG receipts[3])
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    HANDLE values[3]={message->u.CheckVDM.StdIn,message->u.CheckVDM.StdOut,
        message->u.CheckVDM.StdErr};
    ULONG roles[3]={BROKER_VDM_STDIN,BROKER_VDM_STDOUT,BROKER_VDM_STDERR};
    DWORD standard_ids[3]={STD_INPUT_HANDLE,STD_OUTPUT_HANDLE,STD_ERROR_HANDLE};
    DWORD present_mask=0u,original_console_mask=0u,current_console_mask=0u;
    ULONG index,previous;
    DWORD type;
    DWORD error=ERROR_SUCCESS;
    if (!receipts) return ERROR_INVALID_PARAMETER;
    receipts[0]=receipts[1]=receipts[2]=0;
    for (index=0;index<3;++index) {
        BOOL original_console;
        BOOL current_console;
        if (values[index]) present_mask|=1u<<index;
        original_console=OpenNtBaseIsConsoleHandle(values[index]);
        current_console=current_standard_console(values[index],standard_ids[index]);
        if (original_console) original_console_mask|=1u<<index;
        if (current_console) current_console_mask|=1u<<index;
        /* Original BaseCheckVDM/CSR represents the interactive Console via
         * its ConsoleRecord, not through the three redirected standard-stream
         * fields.  The modern Console exposes ordinary Win32 handles, so the
         * cross-process carrier must recognize them before attempting a file
         * attachment.  Pipes and files deliberately remain untouched: they
         * are the original redirection contract and retain their typed
         * receipts below. */
        if (original_console || current_console) {
            values[index]=NULL;
            continue;
        }
        /* COMMAND's native-child startup can carry the original DOS
         * INVALID_HANDLE_VALUE stdin sentinel.  It denotes no inheritable
         * stream, not an OS resource eligible for typed attachment. */
        if (!values[index] || values[index]==INVALID_HANDLE_VALUE) continue;
        SetLastError(ERROR_SUCCESS);
        type=GetFileType(values[index]);
        if (type==FILE_TYPE_UNKNOWN && GetLastError()!=ERROR_SUCCESS) continue;
        for (previous=0;previous<index;++previous) if (values[previous]==values[index]) {
            receipts[index]=receipts[previous]; break;
        }
        if (previous<index) continue;
        {
            RPC_ASYNC_STATE async;
            error=OpenNtBaseClientFailure(0);
            if(!error)error=common_rpc_begin_async(&async);
            if(!error) {
                RpcTryExcept {
                    if (type==FILE_TYPE_PIPE)
                        Client_AttachPipe(&async,client.binding,client.connection,client.process,
                            client.generation,roles[index],values[index],&receipts[index]);
                    else
                        Client_AttachFile(&async,client.binding,client.connection,client.process,
                            client.generation,roles[index],values[index],&receipts[index]);
                }
                RpcExcept(1) { error=RpcExceptionCode(); }
                RpcEndExcept
                error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                    INFINITE));
            }
        }
        if (error || !receipts[index]) break;
    }
    if (error || index!=3) {
        error=OpenNtBaseClientFailure(error ? error : ERROR_INVALID_HANDLE);
        revoke_streams(&client,receipts);
        receipts[0]=receipts[1]=receipts[2]=0;
        return error ? error : ERROR_INVALID_HANDLE;
    }
    return ERROR_SUCCESS;
}

/* Returned attachments may arrive even when cancellation wins the reply.
 * Transfer consumed handles by clearing their slot; close every other copy. */
static void release_handle_array(HANDLE *values,ULONG count)
{
    ULONG index;
    if(!values)return;
    for(index=0;index<count;++index)if(values[index])CloseHandle(values[index]);
    MIDL_user_free(values);
}

static NTSTATUS check_command(PBASE_CHECKVDM_MSG command)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    BASE_API_MSG forwarded={0};
    unsigned char reply[40];
    void *wire=NULL;
    uint32_t request=(uint32_t)InterlockedIncrement(&request_id),wireBytes=0;
    HANDLE parent_event=NULL,*parent_events=NULL;
    ULONG parent_event_count=0,parent_receipt_value=0,replyBytes=0;
    DWORD error=ERROR_INVALID_DATA;
    BOOL applied=FALSE;
    ULONG streams[3]={0,0,0};
    if (!request) request=(uint32_t)InterlockedIncrement(&request_id);
    forwarded.u.CheckVDM=*command;
    if ((error=attach_standard_streams(&forwarded,streams))!=ERROR_SUCCESS) goto done;
    forwarded.u.CheckVDM.StdIn=(HANDLE)(ULONG_PTR)streams[0];
    forwarded.u.CheckVDM.StdOut=(HANDLE)(ULONG_PTR)streams[1];
    forwarded.u.CheckVDM.StdErr=(HANDLE)(ULONG_PTR)streams[2];
    if (!OpenNtBaseEncodeCheckCommand(&forwarded,request,client.generation,NULL,0,&wireBytes) ||
        !(wire=HeapAlloc(GetProcessHeap(),0,wireBytes)) ||
        !OpenNtBaseEncodeCheckCommand(&forwarded,request,client.generation,wire,wireBytes,&wireBytes))
        goto done;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_Check(&async,client.binding,client.connection,client.process,client.generation,
                    wireBytes,wire,&parent_event_count,&parent_events,&parent_receipt_value,&replyBytes,reply);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                INFINITE));
        }
    }
    if (!error && replyBytes==sizeof(reply) && parent_event_count<=1 &&
        (!parent_event_count || parent_events)) {
        if (parent_event_count) {parent_event=parent_events[0];parent_events[0]=NULL;}
        applied=OpenNtBaseApplyCheckReply(reply,(uint32_t)replyBytes,client.generation,request,&forwarded);
        if (applied && NT_SUCCESS((NTSTATUS)forwarded.ReturnValue)) {
            command->iTask=forwarded.u.CheckVDM.iTask;
            command->VDMState=forwarded.u.CheckVDM.VDMState;
        }
    }
    if (applied && command->VDMState==VDM_PRESENT_AND_READY &&
        parent_event && parent_receipt_value) {
        command->WaitObjectForParent=parent_event;
        parent_event_handle=parent_event;parent_receipt=parent_receipt_value;
        parent_event=NULL;
    } else if (applied && (parent_event || parent_receipt_value)) {
        applied=FALSE;
    }
done:
    release_handle_array(parent_events,parent_event_count);
    if (parent_event) CloseHandle(parent_event);
    if (wire) HeapFree(GetProcessHeap(),0,wire);
    if (error || !applied) {
        error=OpenNtBaseClientFailure(error ? error : ERROR_INVALID_DATA);
        revoke_streams(&client,streams);
        return rpc_failure(error);
    }
    return (NTSTATUS)forwarded.ReturnValue;
}

static NTSTATUS get_command(PBASE_GET_NEXT_VDM_COMMAND_MSG result, HANDLE *wait, ULONGLONG *direct)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    BASE_API_MSG decoded={0};
    PBASE_API_MSG base=&decoded;
    uint32_t request=(uint32_t)InterlockedIncrement(&request_id),wire_bytes=0;
    unsigned char *reply=NULL;
    void *wire=NULL;
    HANDLE wait_event=NULL;
    HANDLE *wait_events=NULL;
    HANDLE *pipe_handles=NULL;
    HANDLE *file_handles=NULL;
    ULONG wait_event_count=0;
    ULONG pipe_mask=0,pipe_count=0,file_mask=0,file_count=0;
    ULONG reply_bytes=0;
    DWORD error=ERROR_INVALID_DATA;
    BOOL applied=FALSE;
    decoded.u.GetNextVDMCommand=*result;
    if (!request) request=(uint32_t)InterlockedIncrement(&request_id);
    if (!OpenNtBaseEncodeGetCommand(base,request,client.generation,NULL,0,&wire_bytes) ||
        !(wire=HeapAlloc(GetProcessHeap(),0,wire_bytes)) ||
        !OpenNtBaseEncodeGetCommand(base,request,client.generation,wire,wire_bytes,&wire_bytes))
        goto done;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_Get(&async,client.binding,client.connection,client.process,client.generation,
                    wire_bytes,wire,&wait_event_count,&wait_events,
                    &pipe_mask,&pipe_count,&pipe_handles,&file_mask,&file_count,&file_handles,
                    &reply_bytes,&reply);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                INFINITE));
        }
    }
    if (!error && wait_event_count<=1 && (!wait_event_count || wait_events) &&
        pipe_count<=3 && (!pipe_count || pipe_handles) &&
        file_count<=3 && (!file_count || file_handles) && reply && reply_bytes) {
        if (wait_event_count) {wait_event=wait_events[0];wait_events[0]=NULL;}
        applied=OpenNtBaseApplyGetCommand(reply,(uint32_t)reply_bytes,client.generation,request,base);
        if(applied)(void)OpenNtBaseReadGetObservation(reply,reply_bytes,client.generation,request,direct);
    }
    if (applied) {
        base->u.GetNextVDMCommand.WaitObjectForVDM=wait_event;
        {
            PBASE_GET_NEXT_VDM_COMMAND_MSG command = &base->u.GetNextVDMCommand;
            HANDLE streams[3] = {command->StdIn, command->StdOut, command->StdErr};
            error = OpenNtBaseAdoptStandardHandles(streams, pipe_mask, pipe_count,
                pipe_handles, file_mask, file_count, file_handles);
            applied = error == ERROR_SUCCESS;
            if (applied) {
                command->StdIn = streams[0];
                command->StdOut = streams[1];
                command->StdErr = streams[2];
            }
        }
        if (applied) {
            *wait=wait_event;
            wait_event=NULL;
        }
    }
done:
    release_handle_array(wait_events,wait_event_count);
    release_handle_array(pipe_handles,pipe_count);
    release_handle_array(file_handles,file_count);
    if (wait_event) CloseHandle(wait_event);
    if (reply) MIDL_user_free(reply);
    if (wire) HeapFree(GetProcessHeap(),0,wire);
    *result=decoded.u.GetNextVDMCommand;
    if (error || !applied) {
        return rpc_failure(error ? error : ERROR_INVALID_DATA);
    }
    return (NTSTATUS)base->ReturnValue;
}

static NTSTATUS exit_command(BOOL is_wow, BOOL notify_started, ULONG task, ULONG *release_wait)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    DWORD error=ERROR_INVALID_PARAMETER;
    ULONG close_worker_wait=0;
    if (notify_started) (void)OpenNtBaseClientWowStarted(task);
    if (is_wow) OpenNtBaseClientForgetWowTask(task);
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_Exit(&async,client.binding,client.connection,client.process,client.generation,
                    is_wow ? 1u : 0u,task,&close_worker_wait);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                INFINITE));
        }
    }
    if (error) return rpc_failure(error);
    *release_wait=close_worker_wait;
    return STATUS_SUCCESS;
}

static NTSTATUS update_command(PBASE_UPDATE_VDM_ENTRY_MSG command)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    BASE_API_MSG decoded={0};
    PBASE_API_MSG base=&decoded;
    unsigned char reply[32];
    void *wire=NULL;
    HANDLE parent_event=NULL,*parent_events=NULL;
    uint32_t request=(uint32_t)InterlockedIncrement(&request_id),wire_bytes=0;
    ULONG parent_event_count=0,reply_bytes=0,receipt=0;
    DWORD error=ERROR_INVALID_DATA;
    BOOL applied=FALSE;
    if (!request) request=(uint32_t)InterlockedIncrement(&request_id);
    decoded.u.UpdateVDMEntry=*command;
    if (!OpenNtBaseEncodeUpdateCommand(base,request,client.generation,NULL,0,&wire_bytes) ||
        !(wire=HeapAlloc(GetProcessHeap(),0,wire_bytes)) ||
        !OpenNtBaseEncodeUpdateCommand(base,request,client.generation,wire,wire_bytes,&wire_bytes))
        goto done;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_Update(&async,client.binding,client.connection,client.process,client.generation,
                    wire_bytes,wire,&parent_event_count,&parent_events,&receipt,&reply_bytes,reply);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                INFINITE));
        }
    }
    if (!error && reply_bytes==sizeof(reply) && parent_event_count<=1 &&
        (!parent_event_count || parent_events)) {
        if (parent_event_count) {parent_event=parent_events[0];parent_events[0]=NULL;}
        applied=OpenNtBaseApplyUpdateReply(reply,(uint32_t)reply_bytes,client.generation,request,base);
    }
    if (applied) {
        command->WaitObjectForParent=parent_event;
        parent_event_handle=parent_event;
        parent_receipt=receipt;
        parent_event=NULL;
    }
done:
    release_handle_array(parent_events,parent_event_count);
    if (parent_event) CloseHandle(parent_event);
    if (wire) HeapFree(GetProcessHeap(),0,wire);
    if (error || !applied) {
        return rpc_failure(error ? error : ERROR_INVALID_DATA);
    }
    return (NTSTATUS)base->ReturnValue;
}

static NTSTATUS exit_code_command(HANDLE parent, ULONG *code)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    DWORD error=ERROR_INVALID_PARAMETER;
    ULONG exit_code=0;
    if (parent!=parent_event_handle || !parent_receipt)
        goto done;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_ExitCode(&async,client.binding,client.connection,client.process,client.generation,
                    parent_receipt,&exit_code);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                INFINITE));
        }
    }
    if (!error) {
        *code=exit_code;
        parent_event_handle=NULL;parent_receipt=0;
        return STATUS_SUCCESS;
    }
done:
    return rpc_failure(error);
}

static NTSTATUS reenter_command(ULONG change)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    DWORD error=ERROR_INVALID_PARAMETER;

    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_Reenter(&async,client.binding,client.connection,client.process,client.generation,
                    change);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                INFINITE));
        }
    }
    if (!error) {
        return STATUS_SUCCESS;
    }
    return rpc_failure(error);
}

DWORD OpenNtBaseClientStartVdmWorker(PCWSTR environment,DWORD characters,DWORD show,
    HANDLE frontend,HANDLE *worker,HANDLE *parent)
{
    RPC_ASYNC_STATE async;
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    DWORD error=ERROR_INVALID_STATE;ULONG receipt=0;
    if(!worker || !parent)return ERROR_INVALID_PARAMETER;
    *worker=*parent=NULL;
    if(!client.connection || !client.binding || !client.process)return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_StartVdmWorker(&async,client.binding,client.connection,client.process,
            client.generation,characters,(WCHAR *)environment,show,frontend,
            worker,parent,&receipt);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,INFINITE));
    if(!error && (!*worker || (!!*parent != !!receipt)))error=ERROR_INVALID_DATA;
    if(error) {
        if(*worker)CloseHandle(*worker);if(*parent)CloseHandle(*parent);
        *worker=*parent=NULL;
    } else {
        /* Same process-local receipt mapping as original Update's RPC facade. */
        parent_event_handle=*parent;parent_receipt=receipt;
    }
    return error;
}

DWORD WINAPI OpenNtBaseClientWowStarted(ULONG task)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    DWORD error=ERROR_INVALID_STATE;
    if (!client.connection) return error;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_WowStarted(&async,client.binding,client.connection,client.process,
                    client.generation,task);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                INFINITE));
        }
    }
    return error;
}

DWORD OpenNtBaseClientWowStartup(HANDLE parent,HANDLE *event,BOOL *started)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    DWORD error=ERROR_INVALID_STATE;
    ULONG value=0;
    if (!event || !started) return ERROR_INVALID_PARAMETER;
    *event=NULL; *started=FALSE;
    if (!client.connection) return error;
    if (!parent || parent!=parent_event_handle || !parent_receipt)
        return ERROR_INVALID_HANDLE;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_WowStartup(&async,client.binding,client.connection,client.process,
                    client.generation,parent_receipt,event,&value);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                INFINITE));
        }
    }
    if (!error && (!*event || value>1)) error=ERROR_INVALID_DATA;
    if (error && *event) { CloseHandle(*event); *event=NULL; }
    if (!error) *started=value!=0;
    return OpenNtBaseClientFailure(error);
}

static NTSTATUS register_wow(ULONG window)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    RPC_ASYNC_STATE async;
    DWORD error=OpenNtBaseClientFailure(0);
    if (!error) error=common_rpc_begin_async(&async);
    if (!error) {
        RpcTryExcept {
            Client_RegisterWowExec(&async,client.binding,client.connection,client.process,
                client.generation,window);
        }
        RpcExcept(1) { error=RpcExceptionCode(); }
        RpcEndExcept
        error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,INFINITE));
    }
    return error ? rpc_failure(error) : STATUS_SUCCESS;
}

static NTSTATUS first_vdm(ULONG *value)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    RPC_ASYNC_STATE async;
    DWORD error=OpenNtBaseClientFailure(0);
    if (!error) error=common_rpc_begin_async(&async);
    if (!error) {
        RpcTryExcept {
            Client_First(&async,client.binding,client.connection,client.process,
                client.generation,value);
        }
        RpcExcept(1) { error=RpcExceptionCode(); }
        RpcEndExcept
        error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,INFINITE));
    }
    return error ? rpc_failure(error) : STATUS_SUCCESS;
}

static NTSTATUS transport_status(DWORD error)
{
    error=OpenNtBaseClientFailure(error);
    return error ? rpc_failure(error) : STATUS_SUCCESS;
}

DWORD NtvdmBaseConnectExecution(DWORD milliseconds)
{
    static const ntvdm_base_transport transport = {
        transport_status, check_command, get_command, update_command, exit_command,
        exit_code_command, reenter_command, register_wow, first_vdm, BaseDecodeVdmRequest, BaseReadVdmCommandState
    };
    DWORD error=OpenNtBaseClientConnectCurrentTimeout(milliseconds);
    if (!error) {
        (void)OpenNtBaseClientExecutionConnection(reset_launch);
        ntvdm_base_bind_transport(&transport);
    }
    return error;
}
