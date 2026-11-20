/* Worker-local transport only. No native Console presentation or guest policy. */
#include "console_client.h"
#include "worker_lifecycle.h"
#include "compat/include/console_text.h"
#include "console_geometry.h"
#include "compat/include/publication.h"
#include "worker-base/input_watch.h"
#include "compat/include/console_endpoint.h"
#include "common/protocol/frontend_protocol.h"
#include "common/base_srv/base_rpc_client.h"
#include "worker-base/connection.h"
#include "compat/include/mvdm_softpc_mouse_bridge.h"
#include <stddef.h>
#include <limits.h>
#include <string.h>

static BOOL decode_input(const console_io_input *wire,INPUT_RECORD *record)
{
    if(wire->type==CONSOLE_INPUT_FRAME_MOUSE) {
        console_frame_mouse_input frame;console_mouse_input mouse;
        if(!ntcon_worker_decode_input(wire,record))return FALSE;
        memcpy(&frame,&record->Event,sizeof(frame));
        mouse.dx=frame.dx;mouse.dy=frame.dy;mouse.width=frame.width;mouse.height=frame.height;
        mouse.buttons=frame.buttons;mouse.action=frame.action;
        /* Adapt only at the device boundary. Original MVDM receives its old
         * relative record and still owns guest position, scaling and IRQs. */
        record->EventType=CONSOLE_INPUT_RELATIVE_MOUSE;
        memcpy(&record->Event,&mouse,sizeof(mouse));return TRUE;
    }
    return ntcon_worker_decode_input(wire,record);
}

typedef struct console_client {
    session *owner;
    compat_console_endpoint endpoint;
    HANDLE ready,stop,shutdown,failure_event;
    worker_base_input_watch *input_watch;
    HANDLE capability;
    mvdm_mouse_bridge mouse;
} console_client;
static DWORD console_activate(console_client *,BOOL);
static void console_input_failed(void *context,DWORD error)
{
    console_client *client=context;
    EnterCriticalSection(&client->endpoint.lock);
    (void)ntcon_worker_client_fail(&client->endpoint.channel,error);
    LeaveCriticalSection(&client->endpoint.lock);
}
static void console_client_destroy(console_client *client)
{
    client->owner->host_context.mouse=NULL;
    client->owner->host_context.console_endpoint=NULL;
    client->owner->console_client=NULL;
    OpenNtBaseClientSetCommandBinding(NULL,NULL);
    if (client->stop) SetEvent(client->stop);
    worker_base_publication_destroy(client->endpoint.publisher);
    worker_base_input_watch_destroy(client->input_watch);
    CloseHandle(client->endpoint.channel.pipe);CloseHandle(client->endpoint.channel.peer);
    ntcon_worker_client_dispose(&client->endpoint.channel);
    if(client->failure_event)CloseHandle(client->failure_event);
    CloseHandle(client->ready);
    if (client->capability) CloseHandle(client->capability);
    if (client->stop) CloseHandle(client->stop);
    if (client->shutdown) CloseHandle(client->shutdown);
    if (client->endpoint.input_identity) CloseHandle(client->endpoint.input_identity);
    if (client->endpoint.output_identity) CloseHandle(client->endpoint.output_identity);
    DeleteCriticalSection(&client->endpoint.lock);
    DeleteCriticalSection(&client->endpoint.palette_lock);
    ntvdm_console_graphics_destroy(client->endpoint.graphics);
    HeapFree(GetProcessHeap(),0,client);
}

void ntvdm_console_client_end(session *owner)
{
    if (owner && owner->console_client) console_client_destroy(owner->console_client);
}

static DWORD console_command_ready(void *context)
{
    console_client *client=context;
    return console_activate(client,TRUE);
}

DWORD ntvdm_console_client_begin(session *owner)
{
    console_client *client;
    DWORD error;
    if (!owner || owner->console_client || owner->host_context.state != SESSION_STATE_ACTIVE)
        return ERROR_INVALID_STATE;
    client=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*client));
    if (!client) return ERROR_NOT_ENOUGH_MEMORY;
    error=worker_base_io_open(&client->endpoint.channel.pipe,&client->endpoint.channel.peer,&client->ready,
        &client->endpoint.channel.generation);
    if (error) { HeapFree(GetProcessHeap(),0,client);return error; }
    client->owner=owner;
    InitializeCriticalSection(&client->endpoint.lock);
    InitializeCriticalSection(&client->endpoint.palette_lock);
    client->endpoint.graphics=ntvdm_console_graphics_create();
    if (!client->endpoint.graphics) { error=GetLastError();console_client_destroy(client);return error; }
    error=OpenNtBaseClientWorkerFrontendCapability(&client->capability);
    if (error) { console_client_destroy(client);return error; }
    /* The authenticated broker grants both transport and shell-out capability.
     * This worker does not resolve or retain a separate frontend relationship. */
    error=ntcon_worker_client_init(&client->endpoint.channel,client->endpoint.channel.pipe,
        client->endpoint.channel.peer,NULL,client->endpoint.channel.generation);
    if(error){console_client_destroy(client);return error;}
    error=ntvdm_worker_lifecycle_fault_event(&client->failure_event);
    if(error){console_client_destroy(client);return error;}
    client->endpoint.channel.failure_event=client->failure_event;
    client->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    client->endpoint.input_identity=CreateEventW(NULL,TRUE,FALSE,NULL);
    client->endpoint.output_identity=CreateEventW(NULL,TRUE,FALSE,NULL);
    if (!client->endpoint.channel.event || !client->stop ||
        !client->endpoint.input_identity || !client->endpoint.output_identity) {
        error=GetLastError();console_client_destroy(client);return error;
    }
    error=worker_base_shutdown_event(&client->shutdown);
    if(error){console_client_destroy(client);return error;}
    client->endpoint.publisher=worker_base_publication_create(compat_console_send_video_copy,&client->endpoint,client->shutdown);
    if(!client->endpoint.publisher){error=GetLastError();console_client_destroy(client);return error;}
    error=ntvdm_console_graphics_attach_publisher(client->endpoint.graphics,client->endpoint.publisher);
    if(error){console_client_destroy(client);return error;}
    client->input_watch=worker_base_input_watch_create(NULL,NULL,NULL,client->stop,
        NULL,NULL,NULL,NULL,console_input_failed,client);
    client->mouse.input_ready=worker_base_input_watch_event(client->input_watch);
    error=client->input_watch ? console_activate(client,TRUE) : GetLastError();
    if (error) {
        console_client_destroy(client);return error;
    }
    owner->host_context.mouse=&client->mouse;
    owner->host_context.console_endpoint=&client->endpoint;
    owner->console_client=client;
    OpenNtBaseClientSetCommandBinding(console_command_ready,client);
    return ERROR_SUCCESS;
}

BOOL ntvdm_console_inherit_launch_capabilities(HANDLE *capability,HANDLE *execution)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    BOOL result=TRUE;
    HANDLE retained=NULL;
    DWORD error;
    if (!capability || !execution) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    *capability=NULL;*execution=NULL;
    if (!client) return TRUE;
    EnterCriticalSection(&client->endpoint.lock);
    if (!client->capability) { SetLastError(ERROR_INVALID_STATE);result=FALSE; }
    else result=DuplicateHandle(GetCurrentProcess(),client->capability,
        GetCurrentProcess(),capability,SYNCHRONIZE,TRUE,0);
    LeaveCriticalSection(&client->endpoint.lock);
    if (!result) return FALSE;
    /* A worker-created native child must receive this worker's verified
     * execution Console, never an old launcher-local handle from guest env. */
    error=OpenNtBaseClientAcquireConsoleContext(*capability,&retained);
    if (!error && !DuplicateHandle(GetCurrentProcess(),retained,GetCurrentProcess(),
        execution,SYNCHRONIZE,TRUE,0)) error=GetLastError();
    if (retained) CloseHandle(retained);
    if (error) {
        CloseHandle(*capability);*capability=NULL;
        SetLastError(error);return FALSE;
    }
    return result;
}

/* Called with the per-session lock held; all operations share one ordered
 * request sequence, including stream chunks and geometry queries. */
static DWORD exchange(console_client *client,console_io_reply *reply)
{
    return ntcon_worker_exchange(&client->endpoint.channel,&client->endpoint.request,reply);
}

static DWORD console_activate(console_client *client,BOOL active)
{
    DWORD error=ERROR_SUCCESS;
    EnterCriticalSection(&client->endpoint.lock);
    if(active && !client->endpoint.channel.pipe) {
        error=worker_base_io_open(&client->endpoint.channel.pipe,&client->endpoint.channel.peer,&client->ready,
            &client->endpoint.channel.generation);
        if(!error)error=ntcon_worker_client_init(&client->endpoint.channel,client->endpoint.channel.pipe,
            client->endpoint.channel.peer,client->stop,client->endpoint.channel.generation);
        if(!error)client->endpoint.channel.failure_event=client->failure_event;
    }
    if(!active && client->endpoint.channel.pipe) {
        console_io_request barrier={0};console_io_reply reply;
        HANDLE pipe=client->endpoint.channel.pipe,peer=client->endpoint.channel.peer;
        DWORD decision=WORKER_IO_KEEP;
        error=worker_base_io_checkpoint(WORKER_IO_CHECKPOINT_PAUSE,0,&decision);
        if(error || decision==WORKER_IO_KEEP) {
            LeaveCriticalSection(&client->endpoint.lock);return error;
        }
        barrier.operation=CONSOLE_IO_BARRIER;
        error=ntcon_worker_call(&client->endpoint.channel,&barrier,&reply);
        if(!error) {
            error=worker_base_io_close(&pipe,&peer,&client->ready);
            /* A rejected release leaves the usable transport intact. Only
             * after physical closure discard its local operation state. */
            if(!pipe) {
                ntcon_worker_client_dispose(&client->endpoint.channel);
                DWORD disabled=worker_base_input_watch_bind(client->input_watch,NULL);
                if(!error)error=disabled;
            }
        }
    }
    if(!error && active) {
        console_io_reply reply;
        int64_t height;
        ZeroMemory(&client->endpoint.request,sizeof(client->endpoint.request));
        client->endpoint.request.operation=CONSOLE_IO_SCREEN_INFO;
        error=ntcon_worker_call(&client->endpoint.channel,&client->endpoint.request,&reply);
        if(!error) {
            height=(int64_t)reply.state.bottom-reply.state.top+1;
            if(height<=0 || height>SHRT_MAX)error=ERROR_INVALID_DATA;
            else error=ntcon_worker_prepare_text(&client->endpoint.channel,
                (COORD){80,ntvdm_console_return_height((SHORT)height)});
        }
        /* Conversion is part of acquisition, before guest producers resume.
         * A failed conversion must not strand the frontend's active owner. */
        if(error) {
            DWORD release=console_activate(client,FALSE);
            if(!release)mvdm_softpc_mouse_reset(&client->mouse);
            else (void)ntcon_worker_client_fail(&client->endpoint.channel,release);
        }
    }
    /* Unlike IRQ cancellation, successful DOS ownership handoff retires the
     * frontend's DOS mouse route and discards its copied relative records.
     * Original nt_block_event_thread has quiesced the event/timer producers
     * before this call. Match that acknowledged retirement locally; the next
     * DOS activation will receive a fresh ENTER. Never reset on failed RPC. */
    if(!error && active)error=worker_base_input_watch_bind(client->input_watch,client->ready);
    if(!error && !active)mvdm_softpc_mouse_reset(&client->mouse);
    LeaveCriticalSection(&client->endpoint.lock);
    return error;
}
BOOL ntvdm_console_set_active(BOOL active)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    DWORD error=client ? console_activate(client,active) : ERROR_SUCCESS;
    SetLastError(error);return error==ERROR_SUCCESS;
}

static BOOL input_operation(HANDLE input,INPUT_RECORD *records,uint64_t *receipts,DWORD length,LPDWORD read,BOOL peek,
    const uint64_t *live,DWORD live_count)
{
    session *owner=session_thread_current();
    console_client *client=owner && compat_console_input_endpoint(input) ? owner->console_client : NULL;
    console_io_reply reply;
    DWORD error,i;
    BOOL ok;
    if (!client) {
        ok=peek ? PeekConsoleInputW(input,records,length,read) :
            ReadConsoleInputW(input,records,length,read);
        if(ok && receipts && read)for(i=0;i<*read;++i)receipts[i]=0;
        return ok;
    }
    if (!read || (!records && length) || live_count>CONSOLE_IO_INPUT_LIVE_CAPACITY ||
        (live_count && (!live || !receipts || peek))) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    *read=0;
    EnterCriticalSection(&client->endpoint.lock);
    ZeroMemory(&client->endpoint.request,offsetof(console_io_request,data));
    client->endpoint.request.operation=peek ? CONSOLE_IO_PEEK_INPUT : CONSOLE_IO_READ_INPUT;
    client->endpoint.request.state.count=min(length,CONSOLE_IO_INPUT_CAPACITY);
    client->endpoint.request.state.mode=receipts ? CONSOLE_IO_INPUT_RETURNABLE : 0;
    client->endpoint.request.bytes=live_count*sizeof(*live);
    client->endpoint.request.data=(BYTE *)live;
    error=exchange(client,&reply);ok=!error && reply.result;
    if (!error && !ok) error=reply.error;
    if (ok && (reply.state.count>client->endpoint.request.state.count ||
        reply.bytes!=reply.state.count*sizeof(console_io_input))) {
        error=ntcon_worker_client_fail(&client->endpoint.channel,ERROR_INVALID_DATA);ok=FALSE;
    }
    for (i=0;ok && i<reply.state.count;i++) {
        console_io_input wire;
        memcpy(&wire,reply.data+i*sizeof(wire),sizeof(wire));
        if (!decode_input(&wire,&records[i])) { error=ntcon_worker_client_fail(&client->endpoint.channel,ERROR_INVALID_DATA);ok=FALSE; }
        if(receipts)receipts[i]=(uint64_t)wire.receipt_low|((uint64_t)wire.receipt_high<<32);
    }
    if (ok) *read=reply.state.count;
    if (ok) {
        error=worker_base_input_watch_ack(client->input_watch);
        if(error)ok=FALSE;
    }
    LeaveCriticalSection(&client->endpoint.lock);
    if (!ok) SetLastError(error);
    return ok;
}
BOOL WINAPI MvdmReadConsoleInputW(HANDLE input,PINPUT_RECORD records,DWORD length,LPDWORD read)
{
    return input_operation(input,records,NULL,length,read,FALSE,NULL,0);
}
BOOL WINAPI MvdmPeekConsoleInputW(HANDLE input,PINPUT_RECORD records,DWORD length,LPDWORD read)
{
    return input_operation(input,records,NULL,length,read,TRUE,NULL,0);
}
BOOL ntvdm_console_read_input_tagged(HANDLE input,PINPUT_RECORD records,uint64_t *receipts,
    DWORD length,LPDWORD read,BOOL peek,const uint64_t *live,DWORD live_count)
{
    return input_operation(input,records,receipts,length,read,peek,live,live_count);
}
