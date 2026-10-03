#include "console_channel.h"
#include "console_frontend.h"
#include "common/transport/pipe_transfer.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include <stddef.h>
#include <stdio.h>

struct run16_console_channel {
    run16_console_frontend console;
    HANDLE pipe,worker,stop,thread,io_event,ready;
    BOOL input_pending;
    BOOL snapshot_held;
    HANDLE publication_surface;
    DWORD publication_terminal_error;
    SMALL_RECT publication_window;
    run16_console_video committed_video;
    char title[CONSOLE_IO_TITLE_BYTES];
    BOOL title_valid;
    run16_native_frontend *root;
};
static DWORD activate(void *context,BOOL active)
{
    run16_console_channel *channel=context;
    DWORD error;
    /* NTSRV has already granted the only connection. A conflicting local
     * binding is an invariant failure, not permission to queue or arbitrate. */
    error=run16_native_frontend_bind(channel->root,channel,active);
    if(!error && active) {
        HANDLE logical=NULL;
        error=run16_native_frontend_logical_console(channel->root,&logical);
        if(!error) {
            CloseHandle(channel->console.output);
            channel->console.output=logical;
        } else (void)run16_native_frontend_bind(channel->root,channel,FALSE);
    }
    /* Keep the shared logical viewport through acquisition. Only a copied
     * geometry operation or publication changes its dimensions. */
    if(!error && active && channel->title_valid &&
        !run16_native_frontend_enter(channel->root,channel)) {
        run16_native_frontend_worker_title(channel->root,channel,channel->title);
        run16_native_frontend_leave(channel->root);
    }
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
static DWORD reject_pipe_activation(void *context,BOOL active)
{
    (void)context;(void)active;
    /* Connection ownership is granted by authenticated NTSRV RPC only. */
    return ERROR_NOT_SUPPORTED;
}
static DWORD prepare_text(void *context,COORD size)
{
    run16_console_channel *channel=context;
    HANDLE logical=NULL;
    BOOL committed=FALSE;
    DWORD error=run16_native_frontend_prepare_text(channel->root,channel,size,&committed);
    if(!error)error=run16_native_frontend_logical_console(channel->root,&logical);
    if(!error){CloseHandle(channel->console.output);channel->console.output=logical;}
    else if(committed)channel->publication_terminal_error=error;
    return error;
}
static DWORD video_data(void *context,uint32_t serial,uint32_t offset,const void *data,uint32_t bytes)
{
    run16_console_channel *channel=context;
    run16_console_video *video=&channel->console.video;
    HANDLE logical=NULL;
    DWORD error;
    /* Explicit batches already own a private grid and frame. Standalone
     * publications need the same old-until-commit guarantee. */
    if(channel->publication_surface)
        return run16_console_video_data(video,serial,offset,data,bytes);
    error=run16_console_video_stage_data(video,serial,offset,data,bytes);
    if(error || !video->pending_validated)return error;
    error=run16_native_frontend_video(channel->root,channel,video,TRUE);
    if(error) {
        if(video->pending)run16_console_video_abort_pending(video); /* Precommit failure. */
        else channel->publication_terminal_error=error; /* Projection failed after commit. */
        return error;
    }
    if(video->pixels && video->description.kind==CONSOLE_VIDEO_TEXT_FRAME) {
        error=run16_native_frontend_logical_console(channel->root,&logical);
        if(error){channel->publication_terminal_error=error;return error;}
        CloseHandle(channel->console.output);channel->console.output=logical;
    }
    return ERROR_SUCCESS;
}
static DWORD enter(void *context)
{
    run16_console_channel *channel=context;
    return run16_native_frontend_enter(channel->root,channel);
}
static void leave(void *context)
{
    run16_console_channel *channel=context;
    run16_native_frontend_leave(channel->root);
}
static DWORD screen_begin(void *context)
{
    /* Dispatch already holds enter's I/O lock. The callback pair retains
     * the dispatch failure contract; there is no second screen mutex. */
    (void)context;return ERROR_SUCCESS;
}
static DWORD screen_end(void *context,BOOL write)
{
    run16_console_channel *channel=context;
    return write && !channel->publication_surface ?
        run16_native_frontend_project_text(channel->root) : ERROR_SUCCESS;
}
static DWORD publication(void *context,uint32_t operation)
{
    run16_console_channel *channel=context;
    DWORD error;
    BOOL committed;
    if(operation==CONSOLE_IO_PUBLICATION_BEGIN) {
        if(channel->publication_terminal_error)return ERROR_INVALID_STATE;
        if(channel->publication_surface)return ERROR_BUSY;
        error=run16_native_frontend_clone_text(channel->root,&channel->publication_surface,
            &channel->publication_window);
        if(error)return error;
        /* Save the last committed frame; a failed tile stream cannot mutate
         * the object still borrowed by the renderer. */
        channel->committed_video=channel->console.video;
        error=run16_native_frontend_video(channel->root,channel,&channel->committed_video,FALSE);
        if(error) {
            CloseHandle(channel->publication_surface);channel->publication_surface=NULL;
            ZeroMemory(&channel->committed_video,sizeof(channel->committed_video));return error;
        }
        ZeroMemory(&channel->console.video,sizeof(channel->console.video));
        channel->console.video.serial=channel->committed_video.serial;
        CloseHandle(channel->console.output);
        channel->console.output=channel->publication_surface;
        channel->console.logical_window=&channel->publication_window;
        return ERROR_SUCCESS;
    }
    if(!channel->publication_surface)return ERROR_INVALID_STATE;
    if(operation==CONSOLE_IO_PUBLICATION_END &&
        (channel->console.video.pending || !channel->console.video.pixels))return ERROR_INVALID_DATA;
    if(operation==CONSOLE_IO_PUBLICATION_END) {
        CONSOLE_SCREEN_BUFFER_INFO info;
        const console_video_description *frame=&channel->console.video.description;
        const console_text_style *style=(const console_text_style *)channel->console.video.pixels;
        if(!GetConsoleScreenBufferInfo(channel->publication_surface,&info))return GetLastError();
        if(channel->publication_window.Left<0 || channel->publication_window.Top<0 ||
            channel->publication_window.Right>=info.dwSize.X || channel->publication_window.Bottom>=info.dwSize.Y ||
            frame->kind!=CONSOLE_VIDEO_TEXT_FRAME ||
            style->cursor_column!=info.dwCursorPosition.X-channel->publication_window.Left ||
            style->cursor_row!=info.dwCursorPosition.Y-channel->publication_window.Top ||
            frame->width!=(uint32_t)(channel->publication_window.Right-channel->publication_window.Left+1) ||
            frame->height!=(uint32_t)(channel->publication_window.Bottom-channel->publication_window.Top+1))
            return ERROR_INVALID_DATA;
        /* Validate font/revision/notification before consuming staging. Grid,
         * frame and font then use the same commit as standalone publication. */
        error=run16_native_frontend_publish_text(channel->root,channel,&channel->console.video,
            channel->publication_surface,channel->publication_window,&committed);
        if(!committed)return error; /* Staging can still be explicitly aborted. */
        channel->publication_surface=NULL;channel->console.output=NULL;
        run16_console_video_dispose(&channel->committed_video);
        if(error)channel->publication_terminal_error=error;
    } else {
        uint32_t attempted_serial=channel->console.video.serial;
        CloseHandle(channel->publication_surface);
        channel->publication_surface=NULL;channel->console.output=NULL;
        run16_console_video_dispose(&channel->console.video);
        channel->console.video=channel->committed_video;
        if(channel->console.video.serial<attempted_serial)channel->console.video.serial=attempted_serial;
        ZeroMemory(&channel->committed_video,sizeof(channel->committed_video));
        error=run16_native_frontend_video(channel->root,channel,&channel->console.video,FALSE);
    }
    channel->console.logical_window=run16_native_frontend_text_region(channel->root);
    {
        DWORD handle_error=run16_native_frontend_logical_console(channel->root,&channel->console.output);
        if(handle_error)channel->publication_terminal_error=handle_error;
        if(!error)error=handle_error;
    }
    return error;
}
static DWORD snapshot_begin(void *context)
{
    run16_console_channel *channel=context;
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
static void publish_title(void *context,const char *title)
{
    run16_console_channel *channel=context;
    strcpy_s(channel->title,sizeof(channel->title),title);
    channel->title_valid=TRUE;
    if(!run16_native_frontend_enter(channel->root,channel)) {
        run16_native_frontend_worker_title(channel->root,channel,channel->title);
        run16_native_frontend_leave(channel->root);
    }
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
    return run16_native_frontend_read(channel->root,peek,records,count,read);
}
static DWORD prepend_input(void *context,const INPUT_RECORD *records,DWORD count)
{
    run16_console_channel *channel=context;
    return run16_native_frontend_prepend(channel->root,records,count);
}

/* Explicit cancellation plus peer death. Never close an OVERLAPPED event or
 * buffer while the kernel may still complete its outstanding operation. */
static DWORD transfer(run16_console_channel *channel,BOOL write,void *buffer,DWORD bytes)
{
    BYTE *cursor=buffer;
    while (bytes) {
        common_pipe_operation io;
        DWORD done=0,error,wait;
        HANDLE waits[4]={channel->stop,channel->worker,channel->io_event,run16_native_frontend_ready(channel->root)};
        if (WaitForSingleObject(channel->stop,0)==WAIT_OBJECT_0) return ERROR_OPERATION_ABORTED;
        error=common_pipe_begin(&io,channel->pipe,channel->io_event,write,cursor,bytes,bytes,&done);
        if (error) {
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

static DWORD WINAPI console_channel_main(void *context)
{
    run16_console_channel *channel=context;
    console_io_request *request=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*request));
    console_io_reply reply={0};
    DWORD error=request ? ERROR_SUCCESS : ERROR_NOT_ENOUGH_MEMORY;
    while (!error) {
        BOOL video_locked=FALSE;
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
        if(!error && channel->publication_surface &&
            (request->operation==CONSOLE_IO_ACTIVATE || request->operation==CONSOLE_IO_PREPARE_TEXT_REGION ||
             request->operation==CONSOLE_IO_SNAPSHOT_BEGIN ||
             request->operation==CONSOLE_IO_READ_INPUT || request->operation==CONSOLE_IO_PEEK_INPUT))
            error=ERROR_INVALID_DATA;
        if(!error && (request->operation==CONSOLE_IO_VIDEO_BEGIN ||
            request->operation==CONSOLE_IO_VIDEO_DATA || request->operation==CONSOLE_IO_VIDEO_TEXT)) {
            /* Dispatch commits frame and dependent grid at the final chunk.
             * Keep its logical-grid import and duplicate update in that same
             * critical section; the renderer must not see new-frame/old-grid. */
            run16_native_frontend_snapshot_begin(channel->root);video_locked=TRUE;
        }
        if (!error) error=run16_console_dispatch(&channel->console,request,&reply);
        if(!error && reply.result && !channel->publication_surface && !channel->console.video.pending && (request->operation==CONSOLE_IO_VIDEO_BEGIN ||
            request->operation==CONSOLE_IO_VIDEO_TEXT))
            error=run16_native_frontend_video(channel->root,channel,&channel->console.video,TRUE);
        if(video_locked)run16_native_frontend_snapshot_end(channel->root);
        if (!error && (request->operation==CONSOLE_IO_READ_INPUT ||
            request->operation==CONSOLE_IO_PEEK_INPUT || request->operation==CONSOLE_IO_ACTIVATE)) {
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
        if(!error && channel->publication_terminal_error)error=channel->publication_terminal_error;
    }
    if(channel->snapshot_held)(void)snapshot_end(channel);
    if(channel->publication_surface) {
        /* The endpoint is failed; abort staging even if it lost ownership.
         * No publication callback may invoke activation during EOF cleanup. */
        run16_native_frontend_snapshot_begin(channel->root);
        (void)publication(channel,CONSOLE_IO_PUBLICATION_ABORT);
        run16_native_frontend_snapshot_end(channel->root);
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

DWORD run16_console_channel_stop(run16_console_channel *channel)
{
    DWORD wait;
    if (!channel) return ERROR_SUCCESS;
    if (channel->stop) {
        SetEvent(channel->stop);
    }
    if (channel->thread) {
        /* Signal-driven pipe transfers observe stop and cancel/drain their
         * own OVERLAPPED before closing the pipe. Cancel only synchronous
         * Console work here: the thread may already have closed its pipe,
         * so this borrowed handle value cannot safely be used for CancelIoEx.
         * Then bound
         * the join. A timed-out thread still borrows channel/root storage;
         * the caller must keep both alive until terminal process cleanup. */
        (void)CancelSynchronousIo(channel->thread);
        wait=WaitForSingleObject(channel->thread,10000);
        if(wait!=WAIT_OBJECT_0)
            return wait==WAIT_TIMEOUT ? ERROR_TIMEOUT : GetLastError();
        CloseHandle(channel->thread);
    } else if (channel->pipe && channel->pipe!=INVALID_HANDLE_VALUE) CloseHandle(channel->pipe);
    run16_native_frontend_forget(channel->root,channel);
    run16_console_video_dispose(&channel->console.video);
    run16_console_video_dispose(&channel->committed_video);
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
    channel->console.activate=reject_pipe_activation;channel->console.prepare_text=prepare_text;
    channel->console.video_data=video_data;
    channel->console.enter=enter;channel->console.leave=leave;
    channel->console.screen_begin=screen_begin;channel->console.screen_end=screen_end;
    channel->console.snapshot_begin=snapshot_begin;channel->console.snapshot_end=snapshot_end;
    channel->console.publication=publication;
    channel->console.text_frame_required=text_frame_required;
    channel->console.window_clip_owned=window_clip_owned;
    channel->console.title_changed=title_changed;
    channel->console.publish_title=publish_title;
    channel->console.read_text_configuration=read_text_configuration;
    channel->console.read_input=read_input;channel->console.prepend_input=prepend_input;
    channel->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    channel->io_event=CreateEventW(NULL,TRUE,FALSE,NULL);
    channel->ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    if (!channel->stop || !channel->io_event || !channel->ready) { error=GetLastError();goto fail; }
    error=run16_native_frontend_console(root,&channel->console.input,&channel->console.output);
    if (error) goto fail;
    CloseHandle(channel->console.output);channel->console.output=NULL;
    run16_native_frontend_snapshot_begin(root);
    error=run16_native_frontend_logical_console(root,&channel->console.output);
    run16_native_frontend_snapshot_end(root);
    if(error)goto fail;
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
    /* Bind the broker-authorized channel before exporting the pipe. A worker
     * receiving the attachment must not race an unbound presentation. */
    error=activate(channel,TRUE);
    if(error)goto fail;
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
