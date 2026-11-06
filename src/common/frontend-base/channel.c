#include "channel.h"
#include "lib-win32/pipe/pipe_transfer.h"
#include "lib-win32/pipe/pipe_pair.h"
#include <stdio.h>

struct frontend_base_channel {
    HANDLE pipe,worker,stop,thread,io_event,ready,input_ready;
    BOOL input_pending;
    frontend_base_channel_ops ops;
    void *context;
};
static DWORD transfer(frontend_base_channel *channel,BOOL write,void *buffer,DWORD bytes)
{
    BYTE *cursor=buffer;
    while (bytes) {
        common_pipe_operation io;
        DWORD done=0,error,wait;
        HANDLE waits[4]={channel->stop,channel->worker,channel->io_event,channel->input_ready};
        if (WaitForSingleObject(channel->stop,0)==WAIT_OBJECT_0) return ERROR_OPERATION_ABORTED;
        error=common_pipe_begin(&io,channel->pipe,channel->io_event,write,cursor,bytes,bytes,&done);
        if (error) {
            if (error!=ERROR_IO_PENDING) return error;
            for (;;) {
                wait=WaitForMultipleObjects(!write && !channel->input_pending && channel->ops.active(channel->context) ? 4 : 3,waits,FALSE,INFINITE);
                if (wait!=WAIT_OBJECT_0+3) break;
                if (!SetEvent(channel->ready)) { wait=WAIT_FAILED;break; }
                channel->input_pending=TRUE;
            }
            if (wait!=WAIT_OBJECT_0+2) {
                error=wait==WAIT_FAILED ? GetLastError() :
                    wait==WAIT_OBJECT_0 ? ERROR_OPERATION_ABORTED : ERROR_PROCESS_ABORTED;
                common_pipe_cancel_drain(&io);
                return error;
            }
            error=common_pipe_finish(&io,&done);
            common_pipe_cancel_drain(&io);
            if(error)return error;
        }
        if (!done || done>bytes) return ERROR_BROKEN_PIPE;
        cursor+=done;bytes-=done;
    }
    return ERROR_SUCCESS;
}

static DWORD send_reply(frontend_base_channel *channel,const console_io_reply *reply)
{
    DWORD error;
    const BYTE *payload=console_io_reply_payload((console_io_reply *)reply);
    if(reply->bytes && !payload)return ERROR_INVALID_PARAMETER;
    error=transfer(channel,TRUE,(void *)reply,CONSOLE_IO_REPLY_HEADER_BYTES);
    if(!error && reply->bytes)error=transfer(channel,TRUE,(void *)payload,reply->bytes);
    return error;
}


static DWORD WINAPI channel_main(void *context)
{
    frontend_base_channel *channel=context;
    const frontend_base_cancel cancel={channel->stop,channel->worker};
    console_io_request *request=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*request));
    console_io_reply reply={0};
    BYTE reply_data[CONSOLE_IO_TILE_BYTES];
    BYTE *request_data=NULL;
    DWORD capacity=0,error=request ? ERROR_SUCCESS : ERROR_NOT_ENOUGH_MEMORY;
    while(!error) {
        frontend_base_result result={TRUE,0};
        error=transfer(channel,FALSE,request,CONSOLE_IO_REQUEST_HEADER_BYTES);
        if(error)break;
        ZeroMemory(&reply,sizeof(reply));reply.data=reply_data;
        if(request->reserved || request->bytes>CONSOLE_IO_MAX_DATA_BYTES) {
            error=ERROR_INVALID_DATA;break;
        }
        if(request->bytes>capacity) {
            BYTE *grown=request_data ? HeapReAlloc(GetProcessHeap(),0,request_data,request->bytes) :
                HeapAlloc(GetProcessHeap(),0,request->bytes);
            if(!grown){error=ERROR_NOT_ENOUGH_MEMORY;break;}
            request_data=grown;capacity=request->bytes;
        }
        request->data=request_data;
        error=request->bytes ? transfer(channel,FALSE,request_data,request->bytes) : ERROR_SUCCESS;
        if(error)break;
        error=channel->ops.dispatch(channel->context,&cancel,request,&reply,&result);
        if(!error && (request->operation==CONSOLE_IO_READ_INPUT ||
            request->operation==CONSOLE_IO_PEEK_INPUT)) {
            /* Reset before peeking: the host readiness event covers an arrival
             * after that peek, including input from a different presentation. */
            if(!ResetEvent(channel->ready))error=GetLastError();
            if(!error)error=channel->ops.input_pending(channel->context,&channel->input_pending);
            if(!error && channel->input_pending && !SetEvent(channel->ready))error=GetLastError();
        }
        if(!error && result.reply)error=send_reply(channel,&reply);
        if(!error && result.terminal_error)error=result.terminal_error;
    }
    channel->ops.finish(channel->context);
    if(request_data)HeapFree(GetProcessHeap(),0,request_data);
    if(request)HeapFree(GetProcessHeap(),0,request);
    CloseHandle(channel->pipe);
    SetEvent(channel->ready);
    return error;
}
HANDLE frontend_base_channel_thread(frontend_base_channel *channel)
{return channel ? channel->thread : NULL;}
HANDLE frontend_base_channel_ready(frontend_base_channel *channel)
{return channel ? channel->ready : NULL;}
DWORD frontend_base_channel_stop(frontend_base_channel *channel)
{
    DWORD wait;
    if(!channel)return ERROR_SUCCESS;
    if(channel->stop && !SetEvent(channel->stop))return GetLastError();
    if(channel->thread) {
        /* The thread cancels/drains OVERLAPPED before closing its pipe.
         * Never cancel through the stale pipe value after thread exit. */
        (void)CancelSynchronousIo(channel->thread);
        wait=WaitForSingleObject(channel->thread,INFINITE);
        if(wait!=WAIT_OBJECT_0)return GetLastError();
        CloseHandle(channel->thread);
    } else if(channel->pipe && channel->pipe!=INVALID_HANDLE_VALUE)CloseHandle(channel->pipe);
    if(channel->io_event)CloseHandle(channel->io_event);
    if(channel->ready)CloseHandle(channel->ready);
    if(channel->stop)CloseHandle(channel->stop);
    if(channel->worker)CloseHandle(channel->worker);
    HeapFree(GetProcessHeap(),0,channel);
    return ERROR_SUCCESS;
}
DWORD frontend_base_channel_start(DWORD request,HANDLE worker,HANDLE input_ready,
    const frontend_base_channel_ops *ops,void *context,frontend_base_channel **output)
{
    frontend_base_channel *channel;
    HANDLE server=INVALID_HANDLE_VALUE;
    WCHAR name[96];
    DWORD error,pid=0,generation=0;
    static LONG serial; /* Process-unique pipe name only, not instance state. */
    if(!output || !request || !worker || worker==INVALID_HANDLE_VALUE || !input_ready ||
        !ops || !ops->bind || !ops->attach || !ops->generation || !ops->active ||
        !ops->dispatch || !ops->input_pending || !ops->finish) {
        if(worker && worker!=INVALID_HANDLE_VALUE)CloseHandle(worker);
        if(output)*output=NULL;
        return ERROR_INVALID_PARAMETER;
    }
    *output=NULL;
    channel=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*channel));
    if(!channel){CloseHandle(worker);return ERROR_NOT_ENOUGH_MEMORY;}
    channel->worker=worker;channel->context=context;channel->ops=*ops;
    channel->input_ready=input_ready;
    channel->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    channel->io_event=CreateEventW(NULL,TRUE,FALSE,NULL);
    channel->ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!channel->stop || !channel->io_event || !channel->ready){error=GetLastError();goto fail;}
    swprintf_s(name,96,L"\\\\.\\pipe\\ntvdm-console-%lu-%lu",GetCurrentProcessId(),
        (DWORD)InterlockedIncrement(&serial));
    error=win32_pipe_pair_create(name,channel->io_event,&server,&channel->pipe);
    if(error)goto fail;
    if(!GetNamedPipeClientProcessId(server,&pid) || pid!=GetCurrentProcessId()){
        error=ERROR_ACCESS_DENIED;goto fail;
    }
    error=ops->bind(context);if(error)goto fail;
    error=ops->attach(context,request,server,channel->ready,&generation);if(error)goto fail;
    ops->generation(context,generation);
    CloseHandle(server);server=INVALID_HANDLE_VALUE;
    channel->thread=CreateThread(NULL,0,channel_main,channel,0,NULL);
    if(!channel->thread){error=GetLastError();goto fail;}
    *output=channel;return ERROR_SUCCESS;
fail:
    if(server!=INVALID_HANDLE_VALUE)CloseHandle(server);
    (void)frontend_base_channel_stop(channel);
    return error;
}
