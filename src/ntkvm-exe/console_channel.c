#include "console_channel.h"
#include "console_frontend.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include <stddef.h>
#include <stdio.h>

struct run16_console_channel {
    run16_console_frontend console;
    HANDLE pipe,worker,stop,thread,io_event,ready;
    BOOL input_pending;
    BOOL kind_selected,native;
    BOOL snapshot_held;
    run16_native_frontend *root;
};
static DWORD activate(void *context,BOOL active,DWORD kind)
{
    run16_console_channel *channel=context;
    DWORD error;
    ULONGLONG deadline=GetTickCount64()+10000;
    if(channel->kind_selected && channel->native!=(kind==CONSOLE_IO_WORKER_NATIVE))return ERROR_INVALID_DATA;
    channel->kind_selected=TRUE;channel->native=kind==CONSOLE_IO_WORKER_NATIVE;
    do {
        error=channel->native ? run16_native_frontend_native_bind(channel->root,channel,active) :
            run16_native_frontend_dos_bind(channel->root,channel,active);
        if(error==ERROR_BUSY && active && !channel->native) {
            ULONGLONG now=GetTickCount64();
            if(now>=deadline){error=ERROR_TIMEOUT;break;}
            error=run16_native_frontend_wait_dos_ready(channel->root,channel,
                channel->stop,(DWORD)(deadline-now));
            if(!error)continue;
        }
        break;
    } while(TRUE);
    if(!error && active && !channel->native) {
        HANDLE logical=NULL;
        error=run16_native_frontend_dos_console(channel->root,&logical);
        if(!error) {
            CloseHandle(channel->console.output);
            channel->console.output=logical;
        } else (void)run16_native_frontend_dos_bind(channel->root,channel,FALSE);
    }
    if(!error && active && channel->native)channel->console.logical_window=NULL;
    if(error && active && !channel->native)
        run16_native_frontend_cancel_dos_pending(channel->root,channel);
    /* A released worker's cached frame predates the new owner's geometry.
     * Root binding has detached this pointer under the shared I/O lock. Keep
     * the visible common screen, but require a fresh complete worker frame. */
    if(!error && !active) {
        uint32_t serial=channel->console.video.serial;
        run16_console_video_dispose(&channel->console.video);
        channel->console.video.serial=serial; /* Handoff does not authorize replay. */
    }
    return error;
}
static DWORD enter(void *context)
{
    run16_console_channel *channel=context;
    return run16_native_frontend_dos_enter(channel->root,channel);
}
static void leave(void *context)
{
    run16_console_channel *channel=context;
    run16_native_frontend_dos_leave(channel->root);
}
static DWORD screen_begin(void *context)
{
    return run16_native_frontend_screen_begin(((run16_console_channel *)context)->root);
}
static DWORD screen_end(void *context,BOOL write)
{
    run16_console_channel *channel=context;
    DWORD error=run16_native_frontend_screen_end(channel->root,write);
    if(!error && write && !channel->native)
        error=run16_native_frontend_project_dos(channel->root);
    return error;
}
static DWORD snapshot_begin(void *context)
{
    run16_console_channel *channel=context;
    if(!channel->native)return ERROR_ACCESS_DENIED;
    if(channel->snapshot_held)return ERROR_BUSY;
    run16_native_frontend_snapshot_begin(channel->root);
    channel->snapshot_held=TRUE;return ERROR_SUCCESS;
}
static DWORD snapshot_end(void *context)
{
    run16_console_channel *channel=context;
    if(!channel->snapshot_held)return ERROR_INVALID_STATE;
    channel->snapshot_held=FALSE;
    run16_native_frontend_snapshot_end(channel->root);
    return ERROR_SUCCESS;
}
static BOOL text_frame_required(void *context)
{
    run16_console_channel *channel=context;
    return run16_native_frontend_text_frame_required(channel->root);
}
static BOOL window_clip_owned(void *context)
{
    run16_console_channel *channel=context;
    return run16_native_frontend_window_clip_owned(channel->root);
}
static void title_changed(void *context)
{
    run16_console_channel *channel=context;
    run16_native_frontend_console_title_changed(channel->root);
}
static DWORD read_text_configuration(void *context,DWORD offset,DWORD revision,console_io_reply *reply)
{
    return run16_native_frontend_read_text_configuration(
        ((run16_console_channel *)context)->root,offset,revision,reply);
}
static BOOL active(run16_console_channel *channel)
{
    if(enter(channel))return FALSE;
    leave(channel);return TRUE;
}
static DWORD read_input(void *context,BOOL peek,INPUT_RECORD *records,DWORD count,DWORD *read)
{
    run16_console_channel *channel=context;
    return run16_native_frontend_dos_read(channel->root,peek,records,count,read);
}
static DWORD prepend_input(void *context,const INPUT_RECORD *records,DWORD count)
{
    run16_console_channel *channel=context;
    return run16_native_frontend_dos_prepend(channel->root,records,count);
}

/* Explicit cancellation plus peer death. Never close an OVERLAPPED event or
 * buffer while the kernel may still complete its outstanding operation. */
static DWORD transfer(run16_console_channel *channel,BOOL write,void *buffer,DWORD bytes)
{
    BYTE *cursor=buffer;
    while (bytes) {
        OVERLAPPED io={0};
        DWORD done=0,error,wait;
        HANDLE waits[4]={channel->stop,channel->worker,channel->io_event,run16_native_frontend_dos_ready(channel->root)};
        BOOL ok;
        if (WaitForSingleObject(channel->stop,0)==WAIT_OBJECT_0) return ERROR_OPERATION_ABORTED;
        ResetEvent(channel->io_event);io.hEvent=channel->io_event;
        ok=write ? WriteFile(channel->pipe,cursor,bytes,&done,&io) :
            ReadFile(channel->pipe,cursor,bytes,&done,&io);
        if (!ok) {
            error=GetLastError();
            if (error!=ERROR_IO_PENDING) return error;
            for (;;) {
                wait=WaitForMultipleObjects(!write && !channel->input_pending && active(channel) ? 4 : 3,waits,FALSE,INFINITE);
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
    console_io_reply reply={0};
    DWORD error=request ? ERROR_SUCCESS : ERROR_NOT_ENOUGH_MEMORY;
    while (!error) {
        error=transfer(channel,FALSE,request,(DWORD)offsetof(console_io_request,data));
        if (error) break;
        if (request->bytes>CONSOLE_IO_DATA_BYTES) { error=ERROR_INVALID_DATA;break; }
        error=transfer(channel,FALSE,request->data,request->bytes);
        /* No activation, input wait or publication is legal while this
         * channel holds the screen lock across the native read tiles. In
         * particular, an activation would wait for the presentation thread
         * which is itself waiting for this lock. Protocol misuse closes the
         * endpoint; the thread's EOF cleanup releases the lock. */
        if(!error && channel->snapshot_held &&
            request->operation!=CONSOLE_IO_SCREEN_INFO &&
            request->operation!=CONSOLE_IO_GET_CURSOR_INFO &&
            request->operation!=CONSOLE_IO_READ_CELLS_W &&
            request->operation!=CONSOLE_IO_READ_TEXT_CONFIGURATION &&
            request->operation!=CONSOLE_IO_SNAPSHOT_END)error=ERROR_INVALID_DATA;
        if(!error && channel->native && request->operation==CONSOLE_IO_VIDEO_BEGIN &&
            (request->bytes!=sizeof(console_video_description) ||
             ((const console_video_description *)request->data)->kind!=CONSOLE_VIDEO_TEXT_FRAME))
            error=ERROR_INVALID_DATA;
        if (!error) error=run16_console_dispatch(&channel->console,request,&reply);
        if(!error && reply.result && !channel->console.video.pending && (request->operation==CONSOLE_IO_VIDEO_BEGIN ||
            request->operation==CONSOLE_IO_VIDEO_DATA || request->operation==CONSOLE_IO_VIDEO_TEXT))
            error=run16_native_frontend_dos_video(channel->root,channel,&channel->console.video);
        if (!error && (request->operation==CONSOLE_IO_READ_INPUT ||
            request->operation==CONSOLE_IO_PEEK_INPUT || request->operation==CONSOLE_IO_DOS_ACTIVE)) {
            INPUT_RECORD record;
            DWORD count=0;
            /* Reset before checking the frontend queue. Its stable readiness
             * event covers arrivals after this peek, including Window input. */
            if (!ResetEvent(channel->ready))error=GetLastError();
            if(!error && !enter(channel)) {
                error=read_input(channel,TRUE,&record,1,&count);
                leave(channel);
            }
            channel->input_pending=count!=0;
            if (!error && count && !SetEvent(channel->ready)) error=GetLastError();
        }
        if (!error) error=transfer(channel,TRUE,&reply,
            (DWORD)offsetof(console_io_reply,data)+reply.bytes);
    }
    if(channel->snapshot_held)(void)snapshot_end(channel);
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

DWORD run16_console_channel_stop(run16_console_channel *channel)
{
    DWORD wait;
    if (!channel) return ERROR_SUCCESS;
    if (channel->stop) {
        SetEvent(channel->stop);
    }
    if (channel->thread) {
        /* Signal-driven pipe transfers observe stop. Also cancel any active
         * synchronous Console API and overlapped pipe I/O once, then bound
         * the join. A timed-out thread still borrows channel/root storage;
         * the caller must keep both alive until terminal process cleanup. */
        (void)CancelSynchronousIo(channel->thread);
        if(channel->pipe && channel->pipe!=INVALID_HANDLE_VALUE)
            (void)CancelIoEx(channel->pipe,NULL);
        wait=WaitForSingleObject(channel->thread,10000);
        if(wait!=WAIT_OBJECT_0)
            return wait==WAIT_TIMEOUT ? ERROR_TIMEOUT : GetLastError();
        CloseHandle(channel->thread);
    } else if (channel->pipe && channel->pipe!=INVALID_HANDLE_VALUE) CloseHandle(channel->pipe);
    run16_native_frontend_dos_forget(channel->root,channel);
    run16_console_video_dispose(&channel->console.video);
    if (channel->io_event) CloseHandle(channel->io_event);
    if (channel->ready) CloseHandle(channel->ready);
    if (channel->stop) CloseHandle(channel->stop);
    if (channel->worker) CloseHandle(channel->worker);
    if (channel->console.input && channel->console.input!=INVALID_HANDLE_VALUE) CloseHandle(channel->console.input);
    if (channel->console.output && channel->console.output!=INVALID_HANDLE_VALUE) CloseHandle(channel->console.output);
    HeapFree(GetProcessHeap(),0,channel);
    return ERROR_SUCCESS;
}

DWORD run16_console_channel_start_request(DWORD request,HANDLE worker,run16_native_frontend *root,run16_console_channel **output)
{
    run16_console_channel *channel;
    HANDLE server=INVALID_HANDLE_VALUE;
    WCHAR name[96];
    OVERLAPPED connect={0};
    DWORD error,client_pid=0,generation=0;
    static LONG serial;
    if (!output || !request || !root || !worker || worker==INVALID_HANDLE_VALUE) {
        if (worker && worker!=INVALID_HANDLE_VALUE) CloseHandle(worker);
        if (output) *output=NULL;
        return ERROR_INVALID_PARAMETER;
    }
    *output=NULL;
    channel=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*channel));
    if (!channel) { if (worker) CloseHandle(worker);return ERROR_NOT_ENOUGH_MEMORY; }
    channel->worker=worker;
    channel->root=root;
    channel->console.logical_window=run16_native_frontend_text_region(root);
    channel->console.io_context=channel;
    channel->console.activate=activate;channel->console.enter=enter;channel->console.leave=leave;
    channel->console.screen_begin=screen_begin;channel->console.screen_end=screen_end;
    channel->console.snapshot_begin=snapshot_begin;channel->console.snapshot_end=snapshot_end;
    channel->console.text_frame_required=text_frame_required;
    channel->console.window_clip_owned=window_clip_owned;
    channel->console.title_changed=title_changed;
    channel->console.read_text_configuration=read_text_configuration;
    channel->console.read_input=read_input;channel->console.prepend_input=prepend_input;
    channel->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    channel->io_event=CreateEventW(NULL,TRUE,FALSE,NULL);
    channel->ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    if (!channel->stop || !channel->io_event || !channel->ready) { error=GetLastError();goto fail; }
    error=run16_native_frontend_console(root,&channel->console.input,&channel->console.output);
    if (error) goto fail;
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
        &generation);
    if (error) goto fail;
    channel->console.generation=generation;
    CloseHandle(server);server=INVALID_HANDLE_VALUE;
    channel->thread=CreateThread(NULL,0,console_channel_main,channel,0,NULL);
    if (!channel->thread) { error=GetLastError();goto fail; }
    *output=channel;
    return ERROR_SUCCESS;
fail:
    if (server!=INVALID_HANDLE_VALUE) CloseHandle(server);
    (void)run16_console_channel_stop(channel);
    return error;
}
