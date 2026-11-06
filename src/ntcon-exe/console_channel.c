#include "common/base_srv/frontend_client.h"
#include "console_channel.h"
#include "console_frontend.h"
#include "common/frontend-base/channel.h"
#include "common/protocol/frontend_lifecycle.h"
#include "common/base_srv/base_rpc_client.h"
#include <stddef.h>
#include <stdio.h>

struct frontend_io_channel {
    common_frontend_key key;
    frontend_console console;
    frontend_base_channel *base;
    BOOL snapshot_held;
    HANDLE publication_surface;
    DWORD publication_terminal_error;
    SMALL_RECT publication_window;
    frontend_video committed_video;
    char title[CONSOLE_IO_TITLE_BYTES];
    BOOL title_valid;
    uint32_t title_epoch;
    frontend_session *root;
};
static DWORD activate(void *context,BOOL active)
{
    frontend_io_channel *channel=context;
    DWORD error;
    /* NTSRV has already granted the only connection. A conflicting local
     * binding is an invariant failure, not permission to queue or arbitrate. */
    error=frontend_session_bind(channel->root,channel,active);
    if(!error && active) {
        HANDLE logical=NULL;
        error=frontend_session_logical_console(channel->root,&logical);
        if(!error) {
            CloseHandle(channel->console.output);
            channel->console.output=logical;
        } else (void)frontend_session_bind(channel->root,channel,FALSE);
    }
    /* Keep the shared logical viewport through acquisition. Only a copied
     * geometry operation or publication changes its dimensions. */
    if(!error && active && channel->title_valid &&
        channel->title_epoch==frontend_session_title_epoch(channel->root) &&
        !frontend_session_enter(channel->root,channel)) {
        frontend_session_worker_title(channel->root,channel,channel->title);
        frontend_session_leave(channel->root);
    }
    /* A released worker's cached frame predates the new owner's geometry.
     * Root binding has detached this pointer under the shared I/O lock. Keep
     * the visible common screen, but require a fresh complete worker frame. */
    if(!error && !active) {
        uint32_t serial=channel->console.video.serial;
        frontend_video_dispose(&channel->console.video);
        channel->console.video.serial=serial; /* Handoff does not authorize replay. */
    }
    return error;
}
static DWORD prepare_text(void *context,COORD size)
{
    frontend_io_channel *channel=context;
    HANDLE logical=NULL;
    BOOL committed=FALSE;
    DWORD error=frontend_session_prepare_text(channel->root,channel,size,&committed);
    if(!error)error=frontend_session_logical_console(channel->root,&logical);
    if(!error){CloseHandle(channel->console.output);channel->console.output=logical;}
    else if(committed)channel->publication_terminal_error=error;
    return error;
}
static DWORD video_data(void *context,uint32_t serial,uint32_t offset,const void *data,uint32_t bytes)
{
    frontend_io_channel *channel=context;
    frontend_video *video=&channel->console.video;
    HANDLE logical=NULL;
    DWORD error;
    /* Explicit batches already own a private grid and frame. Standalone
     * publications need the same old-until-commit guarantee. */
    if(channel->publication_surface)
        return frontend_video_data(video,serial,offset,data,bytes);
    error=frontend_video_stage_data(video,serial,offset,data,bytes);
    if(error || !video->pending_validated)return error;
    error=frontend_session_video(channel->root,channel,video,TRUE);
    if(error) {
        if(video->pending)frontend_video_abort_pending(video); /* Precommit failure. */
        else channel->publication_terminal_error=error; /* Projection failed after commit. */
        return error;
    }
    if(video->pixels && video->description.kind==CONSOLE_VIDEO_TEXT_FRAME) {
        error=frontend_session_logical_console(channel->root,&logical);
        if(error){channel->publication_terminal_error=error;return error;}
        CloseHandle(channel->console.output);channel->console.output=logical;
    }
    return ERROR_SUCCESS;
}
static DWORD enter(void *context)
{
    frontend_io_channel *channel=context;
    return frontend_session_enter(channel->root,channel);
}
static void leave(void *context)
{
    frontend_io_channel *channel=context;
    frontend_session_leave(channel->root);
}
static DWORD screen_begin(void *context)
{
    /* Dispatch already holds enter's I/O lock. The callback pair retains
     * the dispatch failure contract; there is no second screen mutex. */
    (void)context;return ERROR_SUCCESS;
}
static DWORD screen_end(void *context,BOOL write)
{
    frontend_io_channel *channel=context;
    return write && !channel->publication_surface ?
        frontend_session_project_text(channel->root) : ERROR_SUCCESS;
}
static DWORD publication(void *context,uint32_t operation)
{
    frontend_io_channel *channel=context;
    DWORD error;
    BOOL committed;
    if(operation==CONSOLE_IO_PUBLICATION_BEGIN) {
        if(channel->publication_terminal_error)return ERROR_INVALID_STATE;
        if(channel->publication_surface)return ERROR_BUSY;
        error=frontend_session_clone_text(channel->root,&channel->publication_surface,
            &channel->publication_window);
        if(error)return error;
        /* Save the last committed frame; a failed tile stream cannot mutate
         * the object still borrowed by the renderer. */
        channel->committed_video=channel->console.video;
        error=frontend_session_video(channel->root,channel,&channel->committed_video,FALSE);
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
        error=frontend_session_publish_text(channel->root,channel,&channel->console.video,
            channel->publication_surface,channel->publication_window,&committed);
        if(!committed)return error; /* Staging can still be explicitly aborted. */
        channel->publication_surface=NULL;channel->console.output=NULL;
        frontend_video_dispose(&channel->committed_video);
        if(error)channel->publication_terminal_error=error;
    } else {
        uint32_t attempted_serial=channel->console.video.serial;
        CloseHandle(channel->publication_surface);
        channel->publication_surface=NULL;channel->console.output=NULL;
        frontend_video_dispose(&channel->console.video);
        channel->console.video=channel->committed_video;
        if(channel->console.video.serial<attempted_serial)channel->console.video.serial=attempted_serial;
        ZeroMemory(&channel->committed_video,sizeof(channel->committed_video));
        error=frontend_session_video(channel->root,channel,&channel->console.video,FALSE);
    }
    channel->console.logical_window=frontend_session_text_region(channel->root);
    {
        DWORD handle_error=frontend_session_logical_console(channel->root,&channel->console.output);
        if(handle_error)channel->publication_terminal_error=handle_error;
        if(!error)error=handle_error;
    }
    return error;
}
static DWORD snapshot_begin(void *context)
{
    frontend_io_channel *channel=context;
    if(channel->snapshot_held)return ERROR_BUSY;
    frontend_session_snapshot_begin(channel->root);
    channel->snapshot_held=TRUE;return ERROR_SUCCESS;
}
static DWORD snapshot_end(void *context)
{
    frontend_io_channel *channel=context;
    if(!channel->snapshot_held)return ERROR_INVALID_STATE;
    channel->snapshot_held=FALSE;
    frontend_session_snapshot_end(channel->root);
    return ERROR_SUCCESS;
}
static BOOL text_frame_required(void *context)
{
    frontend_io_channel *channel=context;
    return frontend_session_text_frame_required(channel->root);
}
static BOOL window_clip_owned(void *context)
{
    frontend_io_channel *channel=context;
    return frontend_session_window_clip_owned(channel->root);
}
static void title_changed(void *context)
{
    frontend_io_channel *channel=context;
    frontend_session_console_title_changed(channel->root);
}
static DWORD handoff_title(void *context,char *title,DWORD capacity)
{
    frontend_io_channel *channel=context;
    if(channel->title_valid && channel->title_epoch==frontend_session_title_epoch(channel->root)) {
        if(capacity<CONSOLE_IO_TITLE_BYTES)return ERROR_INSUFFICIENT_BUFFER;
        strcpy_s(title,capacity,channel->title);return ERROR_SUCCESS;
    }
    return frontend_session_handoff_title(channel->root,title,capacity);
}
static void publish_title(void *context,const char *title)
{
    frontend_io_channel *channel=context;
    strcpy_s(channel->title,sizeof(channel->title),title);
    channel->title_valid=TRUE;
    channel->title_epoch=frontend_session_title_epoch(channel->root);
    if(!frontend_session_enter(channel->root,channel)) {
        frontend_session_worker_title(channel->root,channel,channel->title);
        frontend_session_leave(channel->root);
    }
}
static DWORD read_text_configuration(void *context,DWORD offset,DWORD revision,console_io_reply *reply)
{
    return frontend_session_read_text_configuration(
        ((frontend_io_channel *)context)->root,offset,revision,reply);
}
static BOOL active(void *context)
{
    frontend_io_channel *channel=context;
    if(enter(channel))return FALSE;
    leave(channel);return TRUE;
}
static DWORD read_input(void *context,BOOL peek,BOOL returnable,INPUT_RECORD *records,uint64_t *receipts,DWORD count,DWORD *read,
    const uint64_t *live,DWORD live_count)
{
    frontend_io_channel *channel=context;
    if(!peek && returnable) {
        DWORD error=frontend_session_input_live(channel->root,live,live_count);
        if(error)return error;
    }
    return frontend_session_read_owned(channel->root,peek,returnable,records,receipts,count,read);
}
static DWORD prepend_input(void *context,const INPUT_RECORD *records,const uint64_t *receipts,DWORD count)
{
    frontend_io_channel *channel=context;
    return frontend_session_prepend_tagged(channel->root,records,receipts,count);
}

static DWORD dispatch(void *context,const frontend_base_cancel *cancel,const console_io_request *request,
    console_io_reply *reply,frontend_base_result *result)
{
    frontend_io_channel *channel=context;
    DWORD error=ERROR_SUCCESS;
    BOOL video_locked=FALSE,video_part=FALSE,video_final=FALSE;
    (void)cancel; /* Existing NTCON presentation owns its cancellation path. */
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
            request->operation!=CONSOLE_IO_GET_HANDOFF_TITLE_A &&
            request->operation!=CONSOLE_IO_SNAPSHOT_END)error=ERROR_INVALID_DATA;
        if(!error && channel->publication_surface &&
            (request->operation==CONSOLE_IO_PREPARE_TEXT_REGION ||
             request->operation==CONSOLE_IO_SNAPSHOT_BEGIN ||
             request->operation==CONSOLE_IO_READ_INPUT || request->operation==CONSOLE_IO_PEEK_INPUT))
            error=ERROR_INVALID_DATA;
        /* A staged video frame owns this output direction until it is
         * complete.  Its bounded non-final parts deliberately carry no
         * individual reply: the sender receives exactly one result when the
         * frame is complete. */
        if(!error && channel->console.video.pending && request->operation!=CONSOLE_IO_VIDEO_DATA)
            error=ERROR_INVALID_DATA;
        /* A framed-record violation abandons the staged frame and closes the
         * endpoint.  Continuing would let a later payload complete a frame
         * after an out-of-order operation, which is neither a valid stream nor
         * a safe recovery point. */
        if(error)return error;
        video_part=request->operation==CONSOLE_IO_VIDEO_DATA;
        if(!error && video_part && channel->console.video.pending &&
            request->state.mode==channel->console.video.pending_serial &&
            request->state.count==channel->console.video.received && request->bytes==
                channel->console.video.pending_description.bytes-channel->console.video.received)
            video_final=TRUE;
        if(!error && (request->operation==CONSOLE_IO_VIDEO_BEGIN ||
            request->operation==CONSOLE_IO_VIDEO_DATA || request->operation==CONSOLE_IO_VIDEO_TEXT)) {
            /* Dispatch commits frame and dependent grid at the final chunk.
             * Keep its logical-grid import and duplicate update in that same
             * critical section; the renderer must not see new-frame/old-grid. */
            frontend_session_snapshot_begin(channel->root);video_locked=TRUE;
        }
        if (!error) error=frontend_console_dispatch(&channel->console,request,reply);
        /* Dispatch has released the frontend I/O lock before this event-driven
         * wait. Only final native CUI handoff asks the presentation thread to
         * acknowledge a particular complete VIDEO serial; ordinary barriers
         * retain their original logical ordering semantics. */
        if(!error && reply->result && request->operation==CONSOLE_IO_PRESENT_BARRIER)
            error=frontend_session_present_barrier(channel->root,channel,&channel->console.video,
                request->state.mode);
        if(!error && reply->result && !channel->publication_surface && !channel->console.video.pending && (request->operation==CONSOLE_IO_VIDEO_BEGIN ||
            request->operation==CONSOLE_IO_VIDEO_TEXT))
            error=frontend_session_video(channel->root,channel,&channel->console.video,TRUE);
        if(video_locked)frontend_session_snapshot_end(channel->root);
        /* Only the sender's final data part receives a reply, including its
         * frame-level failure. A malformed/interrupted non-final part closes
         * the byte stream rather than leaving an unread reply ahead of the
         * final frame result. */
        if(!error && video_part && !video_final && !reply->result) {
            error=reply->error ? reply->error : ERROR_INVALID_DATA;
        }

    result->reply=!video_part || video_final;
    result->terminal_error=channel->publication_terminal_error;
    return error;
}
static DWORD pending_input(void *context,BOOL *pending)
{
    frontend_io_channel *channel=context;
    INPUT_RECORD record;
    DWORD error=ERROR_SUCCESS,count=0;
    if(!enter(channel)){
        error=read_input(channel,TRUE,FALSE,&record,NULL,1,&count,NULL,0);
        leave(channel);
    }
    *pending=count!=0;
    return error;
}
static void finish(void *context)
{
    frontend_io_channel *channel=context;
    if(channel->snapshot_held)(void)snapshot_end(channel);
    if(channel->publication_surface) {
        /* The endpoint is failed; abort staging even if it lost ownership.
         * No publication callback may invoke activation during EOF cleanup. */
        frontend_session_snapshot_begin(channel->root);
        (void)publication(channel,CONSOLE_IO_PUBLICATION_ABORT);
        frontend_session_snapshot_end(channel->root);
    }

}
static DWORD bind_channel(void *context){return activate(context,TRUE);}
static DWORD attach_channel(void *context,DWORD request,HANDLE pipe,HANDLE ready,DWORD *generation)
{
    frontend_io_channel *channel=context;
    return OpenNtBaseClientAttachFrontendRequest(&channel->key,request,pipe,ready,generation);
}
static void set_generation(void *context,DWORD generation)
{((frontend_io_channel *)context)->console.generation=generation;}
HANDLE frontend_io_channel_thread(frontend_io_channel *channel)
{return channel ? frontend_base_channel_thread(channel->base) : NULL;}
DWORD frontend_io_channel_stop(frontend_io_channel *channel)
{
    DWORD error;
    if(!channel)return ERROR_SUCCESS;
    error=frontend_base_channel_stop(channel->base);
    if(error)return error;
    frontend_session_forget(channel->root,channel);
    frontend_video_dispose(&channel->console.video);
    frontend_video_dispose(&channel->committed_video);
    if(channel->console.input && channel->console.input!=INVALID_HANDLE_VALUE)CloseHandle(channel->console.input);
    if(channel->console.output && channel->console.output!=INVALID_HANDLE_VALUE)CloseHandle(channel->console.output);
    HeapFree(GetProcessHeap(),0,channel);
    return ERROR_SUCCESS;
}
DWORD frontend_io_channel_start_request(const common_frontend_key *key,DWORD request,HANDLE worker,frontend_session *root,frontend_io_channel **output)
{
    frontend_io_channel *channel;
    DWORD error;
    static const frontend_base_channel_ops ops={
        bind_channel,attach_channel,set_generation,active,dispatch,pending_input,finish
    };
    if (!key || !key->instance || !key->generation || !output || !request || !root || !worker || worker==INVALID_HANDLE_VALUE) {
        if (worker && worker!=INVALID_HANDLE_VALUE) CloseHandle(worker);
        if (output) *output=NULL;
        return ERROR_INVALID_PARAMETER;
    }
    *output=NULL;
    channel=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*channel));
    if (!channel) { if (worker) CloseHandle(worker);return ERROR_NOT_ENOUGH_MEMORY; }
    channel->key=*key;
    channel->root=root;
    channel->console.logical_window=frontend_session_text_region(root);
    channel->console.io_context=channel;
    channel->console.prepare_text=prepare_text;
    channel->console.video_data=video_data;
    channel->console.enter=enter;channel->console.leave=leave;
    channel->console.screen_begin=screen_begin;channel->console.screen_end=screen_end;
    channel->console.snapshot_begin=snapshot_begin;channel->console.snapshot_end=snapshot_end;
    channel->console.publication=publication;
    channel->console.text_frame_required=text_frame_required;
    channel->console.window_clip_owned=window_clip_owned;
    channel->console.title_changed=title_changed;
    channel->console.publish_title=publish_title;
    channel->console.handoff_title=handoff_title;
    channel->console.read_text_configuration=read_text_configuration;
    channel->console.read_input=read_input;channel->console.prepend_input=prepend_input;
    error=frontend_session_console(root,&channel->console.input,&channel->console.output);
    if (error) goto fail;
    CloseHandle(channel->console.output);channel->console.output=NULL;
    frontend_session_snapshot_begin(root);
    error=frontend_session_logical_console(root,&channel->console.output);
    frontend_session_snapshot_end(root);
    if(error)goto fail;
    error=frontend_base_channel_start(request,worker,frontend_session_ready(root),
        &ops,channel,&channel->base);
    worker=NULL; /* Base consumes the reference even on failure. */
    if(error)goto fail;
    *output=channel;
    return ERROR_SUCCESS;
fail:
    if(worker)CloseHandle(worker);
    (void)frontend_io_channel_stop(channel);
    return error;
}
