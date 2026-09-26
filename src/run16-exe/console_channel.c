#include "console_channel.h"
#include "console_frontend.h"
#include "basesrv-exe/opennt/include/base_rpc_client.h"
#include <stddef.h>
#include <stdio.h>

struct run16_console_channel {
    run16_console_frontend console;
    HANDLE pipe,worker,stop,thread,io_event,ready;
    BOOL input_pending;
};

/* Explicit cancellation plus peer death. Never close an OVERLAPPED event or
 * buffer while the kernel may still complete its outstanding operation. */
static DWORD transfer(run16_console_channel *channel,BOOL write,void *buffer,DWORD bytes)
{
    BYTE *cursor=buffer;
    while (bytes) {
        OVERLAPPED io={0};
        DWORD done=0,error,wait;
        HANDLE waits[4]={channel->stop,channel->worker,channel->io_event,channel->console.input};
        BOOL ok;
        if (WaitForSingleObject(channel->stop,0)==WAIT_OBJECT_0) return ERROR_OPERATION_ABORTED;
        ResetEvent(channel->io_event);io.hEvent=channel->io_event;
        ok=write ? WriteFile(channel->pipe,cursor,bytes,&done,&io) :
            ReadFile(channel->pipe,cursor,bytes,&done,&io);
        if (!ok) {
            error=GetLastError();
            if (error!=ERROR_IO_PENDING) return error;
            for (;;) {
                wait=WaitForMultipleObjects(!write && !channel->input_pending ? 4 : 3,waits,FALSE,INFINITE);
                if (wait!=WAIT_OBJECT_0+3) break;
                if (!SetEvent(channel->ready)) { wait=WAIT_FAILED;break; }
                channel->input_pending=TRUE;
            }
            if (wait!=WAIT_OBJECT_0+2) {
                error=wait==WAIT_FAILED ? GetLastError() :
                    wait==WAIT_OBJECT_0 ? ERROR_OPERATION_ABORTED : ERROR_PROCESS_ABORTED;
                CancelIoEx(channel->pipe,&io);
                (void)GetOverlappedResult(channel->pipe,&io,&done,TRUE);
                return error;
            }
            if (!GetOverlappedResult(channel->pipe,&io,&done,FALSE)) return GetLastError();
        }
        if (!done || done>bytes) return ERROR_BROKEN_PIPE;
        cursor+=done;bytes-=done;
    }
    return ERROR_SUCCESS;
}

static DWORD WINAPI console_channel_main(void *context)
{
    run16_console_channel *channel=context;
    console_io_request *request=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*request));
    console_io_reply reply;
    DWORD error=request ? ERROR_SUCCESS : ERROR_NOT_ENOUGH_MEMORY;
    while (!error) {
        error=transfer(channel,FALSE,request,(DWORD)offsetof(console_io_request,data));
        if (error) break;
        if (request->bytes>CONSOLE_IO_DATA_BYTES) { error=ERROR_INVALID_DATA;break; }
        error=transfer(channel,FALSE,request->data,request->bytes);
        if (!error) error=run16_console_dispatch(&channel->console,request,&reply);
        if (!error && (request->operation==CONSOLE_IO_READ_INPUT ||
            request->operation==CONSOLE_IO_PEEK_INPUT)) {
            INPUT_RECORD record;
            DWORD count=0;
            /* Reset before checking the queue. A later arrival wakes the next
             * pipe wait through CONIN$; it cannot be lost between peek/reset. */
            if (!ResetEvent(channel->ready) ||
                !PeekConsoleInputW(channel->console.input,&record,1,&count)) error=GetLastError();
            channel->input_pending=count!=0;
            if (!error && count && !SetEvent(channel->ready)) error=GetLastError();
        }
        if (!error) error=transfer(channel,TRUE,&reply,
            (DWORD)offsetof(console_io_reply,data)+reply.bytes);
    }
    if (request) HeapFree(GetProcessHeap(),0,request);
    /* This thread owns the endpoint after creation. EOF must reach the worker
     * even when the launcher is still waiting for its original task event. */
    CloseHandle(channel->pipe);
    SetEvent(channel->ready); /* Wake a worker waiter to observe endpoint EOF. */
    return error;
}

HANDLE run16_console_channel_thread(run16_console_channel *channel)
{
    return channel ? channel->thread : NULL;
}

void run16_console_channel_stop(run16_console_channel *channel)
{
    if (!channel) return;
    if (channel->stop) SetEvent(channel->stop);
    if (channel->thread) {
        /* A source caller can request a blocking Console read. Cancel that
         * synchronous public API before joining, as well as pipe I/O below. */
        do {
            CancelSynchronousIo(channel->thread);
        } while (WaitForSingleObject(channel->thread,50)==WAIT_TIMEOUT);
        CloseHandle(channel->thread);
    } else if (channel->pipe && channel->pipe!=INVALID_HANDLE_VALUE) CloseHandle(channel->pipe);
    run16_console_video_dispose(&channel->console.video);
    if (channel->io_event) CloseHandle(channel->io_event);
    if (channel->ready) CloseHandle(channel->ready);
    if (channel->stop) CloseHandle(channel->stop);
    if (channel->worker) CloseHandle(channel->worker);
    if (channel->console.input && channel->console.input!=INVALID_HANDLE_VALUE) CloseHandle(channel->console.input);
    if (channel->console.output && channel->console.output!=INVALID_HANDLE_VALUE) CloseHandle(channel->console.output);
    HeapFree(GetProcessHeap(),0,channel);
}

DWORD run16_console_channel_start_request(DWORD request,HANDLE worker,run16_console_channel **output)
{
    run16_console_channel *channel;
    HANDLE server=INVALID_HANDLE_VALUE;
    WCHAR name[96];
    OVERLAPPED connect={0};
    DWORD error,client_pid=0;
    static LONG serial;
    if (!output || !request || !worker || worker==INVALID_HANDLE_VALUE) {
        if (worker && worker!=INVALID_HANDLE_VALUE) CloseHandle(worker);
        if (output) *output=NULL;
        return ERROR_INVALID_PARAMETER;
    }
    *output=NULL;
    channel=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*channel));
    if (!channel) { if (worker) CloseHandle(worker);return ERROR_NOT_ENOUGH_MEMORY; }
    channel->worker=worker;
    channel->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    channel->io_event=CreateEventW(NULL,TRUE,FALSE,NULL);
    channel->ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    if (!channel->stop || !channel->io_event || !channel->ready) { error=GetLastError();goto fail; }
    channel->console.input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    channel->console.output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if (channel->console.input==INVALID_HANDLE_VALUE || channel->console.output==INVALID_HANDLE_VALUE) {
        error=GetLastError();goto fail;
    }
    swprintf_s(name,96,L"\\\\.\\pipe\\ntvdm-console-%lu-%lu",GetCurrentProcessId(),
        (DWORD)InterlockedIncrement(&serial));
    server=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,32768,32768,0,NULL);
    if (server==INVALID_HANDLE_VALUE) { error=GetLastError();goto fail; }
    channel->pipe=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED|SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION,NULL);
    if (channel->pipe==INVALID_HANDLE_VALUE) { error=GetLastError();goto fail; }
    connect.hEvent=channel->io_event;
    if (!ConnectNamedPipe(server,&connect) && GetLastError()!=ERROR_PIPE_CONNECTED) {
        DWORD ignored;
        error=GetLastError();
        if (error==ERROR_IO_PENDING) {
            CancelIoEx(server,&connect);
            (void)GetOverlappedResult(server,&connect,&ignored,TRUE);
        }
        goto fail;
    }
    if (!GetNamedPipeClientProcessId(server,&client_pid) || client_pid!=GetCurrentProcessId()) {
        error=ERROR_ACCESS_DENIED;goto fail;
    }
    error=OpenNtBaseClientAttachFrontendRequest(request,server,channel->ready,
        &channel->console.generation);
    if (error) goto fail;
    CloseHandle(server);server=INVALID_HANDLE_VALUE;
    channel->thread=CreateThread(NULL,0,console_channel_main,channel,0,NULL);
    if (!channel->thread) { error=GetLastError();goto fail; }
    *output=channel;
    return ERROR_SUCCESS;
fail:
    if (server!=INVALID_HANDLE_VALUE) CloseHandle(server);
    run16_console_channel_stop(channel);
    return error;
}
