/* Project-owned worker client of the copied NTCON protocol, shared by both backends.
 * Resource ownership and backend-specific handoff remain with each caller. */
#include "common/console/client.h"
#include "common/transport/pipe_transfer.h"
#include <stddef.h>
#include <limits.h>
#include <string.h>

DWORD ntcon_worker_client_init(ntcon_worker_client *client,HANDLE pipe,HANDLE peer,HANDLE cancel,DWORD generation)
{
    if(!client || !pipe || pipe==INVALID_HANDLE_VALUE || !peer || !generation)
        return ERROR_INVALID_PARAMETER;
    client->event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!client->event)return GetLastError();
    client->pipe=pipe;client->peer=peer;client->cancel=cancel;client->generation=generation;
    return ERROR_SUCCESS;
}
void ntcon_worker_client_dispose(ntcon_worker_client *client)
{
    if(!client)return;
    if(client->reply_payload)HeapFree(GetProcessHeap(),0,client->reply_payload);
    if(client->event)CloseHandle(client->event);
    ZeroMemory(client,sizeof(*client));
}

DWORD ntcon_worker_activate(ntcon_worker_client *client,BOOL active)
{
    console_io_request request={0};console_io_reply reply;
    request.operation=CONSOLE_IO_ACTIVATE;request.state.input=active!=FALSE;
    return ntcon_worker_call(client,&request,&reply);
}

DWORD ntcon_worker_prepare_text(ntcon_worker_client *client,COORD size)
{
    console_io_request request={0};console_io_reply reply;
    if(size.X<=0 || size.Y<=0)return ERROR_INVALID_PARAMETER;
    request.operation=CONSOLE_IO_PREPARE_TEXT_REGION;
    request.state.width=size.X;request.state.height=size.Y;
    return ntcon_worker_call(client,&request,&reply);
}

DWORD ntcon_worker_prepend_keys(ntcon_worker_client *client,const INPUT_RECORD *records,
    DWORD count,console_io_reply *reply)
{
    console_io_request request={0};console_io_input payload[CONSOLE_IO_INPUT_CAPACITY];DWORD i,error;
    if(!client || !reply || (!records && count) || count>CONSOLE_IO_INPUT_CAPACITY)
        return ERROR_INVALID_PARAMETER;
    request.operation=CONSOLE_IO_PREPEND_KEYS;request.state.count=count;
    request.bytes=count*sizeof(console_io_input);
    for(i=0;i<count;++i) {
        console_io_input wire={0};const KEY_EVENT_RECORD *key=&records[i].Event.KeyEvent;
        if(records[i].EventType!=KEY_EVENT)return ERROR_INVALID_DATA;
        wire.type=KEY_EVENT;wire.key_down=key->bKeyDown!=FALSE;
        wire.repeat=key->wRepeatCount;wire.virtual_key=key->wVirtualKeyCode;
        wire.scan=key->wVirtualScanCode;wire.character=key->uChar.UnicodeChar;
        wire.control=key->dwControlKeyState;
        payload[i]=wire;
    }
    request.data=(BYTE *)payload;
    error=ntcon_worker_exchange(client,&request,reply);
    if(!error && reply->state.count>count)error=client->failure=ERROR_INVALID_DATA;
    return error;
}

static DWORD transfer(ntcon_worker_client *client,BOOL write,void *buffer,DWORD bytes)
{
    return common_pipe_transfer(client->pipe,client->peer,client->cancel,client->event,
        COMMON_PIPE_PEER_DEATH_FIRST,ERROR_PIPE_NOT_CONNECTED,write,buffer,bytes,bytes);
}

static DWORD reserve_reply_payload(ntcon_worker_client *client,console_io_reply *reply)
{
    BYTE *copy;
    if(client->reply_capacity>=reply->bytes) {
        reply->data=client->reply_payload;return ERROR_SUCCESS;
    }
    copy=client->reply_payload ? HeapReAlloc(GetProcessHeap(),0,client->reply_payload,reply->bytes) :
        HeapAlloc(GetProcessHeap(),0,reply->bytes);
    if(!copy)return ERROR_NOT_ENOUGH_MEMORY;
    client->reply_payload=copy;client->reply_capacity=reply->bytes;
    reply->data=copy;return ERROR_SUCCESS;
}

static DWORD send_request(ntcon_worker_client *client,const console_io_request *request)
{
    DWORD error;
    const BYTE *payload=console_io_request_payload(request);
    if(request->bytes && !payload)return ERROR_INVALID_PARAMETER;
    error=transfer(client,TRUE,(void *)request,CONSOLE_IO_REQUEST_HEADER_BYTES);
    if(!error && request->bytes)error=transfer(client,TRUE,(void *)payload,request->bytes);
    return error;
}

static DWORD ntcon_worker_send(ntcon_worker_client *client,console_io_request *request)
{
    DWORD error;
    if(!client || !request || request->bytes>CONSOLE_IO_MAX_DATA_BYTES)
        return ERROR_INVALID_PARAMETER;
    if(client->failure)return client->failure;
    if(!client->pipe)return ERROR_NOT_READY;
    if(client->sequence==UINT32_MAX)return ERROR_ARITHMETIC_OVERFLOW;
    request->version=CONSOLE_IO_VERSION;request->generation=client->generation;
    request->reserved=0;
    request->sequence=++client->sequence;
    error=send_request(client,request);
    if(error==ERROR_BROKEN_PIPE || error==ERROR_NO_DATA || error==ERROR_PIPE_NOT_CONNECTED)
        error=ERROR_PIPE_NOT_CONNECTED;
    if(error)client->failure=error;
    return error;
}

DWORD ntcon_worker_exchange(ntcon_worker_client *client,console_io_request *request,console_io_reply *reply)
{
    DWORD error;
    if(!client || !request || !reply || request->bytes>CONSOLE_IO_MAX_DATA_BYTES)
        return ERROR_INVALID_PARAMETER;
    ZeroMemory(reply,sizeof(*reply));
    if(client->failure)return client->failure;
    /* A disposed connection is an expected unavailable endpoint. Do not
     * mutate its sequence or latch a transfer error: the caller may bind a
     * newly authorized transport after the broker's disconnect barrier. */
    if(!client->pipe)return ERROR_NOT_READY;
    error=ntcon_worker_send(client,request);
    if(!error)error=transfer(client,FALSE,reply,CONSOLE_IO_REPLY_HEADER_BYTES);
    if(!error && (reply->version!=CONSOLE_IO_VERSION || reply->generation!=client->generation ||
        reply->sequence!=client->sequence || reply->result>1 || (reply->result && reply->error) ||
        reply->reserved || reply->padding ||
        reply->bytes>CONSOLE_IO_MAX_DATA_BYTES ||
        (reply->bytes && request->operation!=CONSOLE_IO_GET_TITLE_A &&
            request->operation!=CONSOLE_IO_KEYBOARD_LAYOUT &&
            request->operation!=CONSOLE_IO_READ_TEXT_CONFIGURATION &&
            request->operation!=CONSOLE_IO_READ_INPUT && request->operation!=CONSOLE_IO_PEEK_INPUT &&
            (request->operation<CONSOLE_IO_READ_CELLS_A || request->operation>CONSOLE_IO_READ_CELLS_W))))
        error=ERROR_INVALID_DATA;
    if(!error)error=reserve_reply_payload(client,reply);
    if(!error && reply->bytes)error=transfer(client,FALSE,console_io_reply_payload(reply),reply->bytes);
    if(error==ERROR_BROKEN_PIPE || error==ERROR_NO_DATA || error==ERROR_PIPE_NOT_CONNECTED)
        error=ERROR_PIPE_NOT_CONNECTED;
    if(error)client->failure=error;
    return error;
}

DWORD ntcon_worker_call(ntcon_worker_client *client,console_io_request *request,console_io_reply *reply)
{
    DWORD error=ntcon_worker_exchange(client,request,reply);
    return error ? error : reply->result ? ERROR_SUCCESS : reply->error ? reply->error : ERROR_GEN_FAILURE;
}

DWORD ntcon_worker_video(ntcon_worker_client *client,const console_video_description *description,const void *pixels)
{
    console_io_request request={0};console_io_reply reply;
    DWORD error,offset=0,count;
    if(!client || (description && (!pixels || !description->bytes)))return ERROR_INVALID_PARAMETER;
    if(client->failure)return client->failure;
    if(!client->pipe)return ERROR_NOT_READY;
    if(client->video_serial==UINT32_MAX)return ERROR_ARITHMETIC_OVERFLOW;
    request.state.mode=++client->video_serial;
    request.operation=description ? CONSOLE_IO_VIDEO_BEGIN : CONSOLE_IO_VIDEO_TEXT;
    if(description) {
        request.bytes=sizeof(*description);request.data=(BYTE *)description;
    }
    error=ntcon_worker_call(client,&request,&reply);
    while(!error && description && offset<description->bytes) {
        count=min(description->bytes-offset,CONSOLE_IO_MAX_DATA_BYTES);
        request.operation=CONSOLE_IO_VIDEO_DATA;request.state.count=offset;request.bytes=count;
        request.data=(BYTE *)pixels+offset;
        /* Larger frames remain bounded parts, but only the final part waits
         * for the one frame-level result.  A peer failure on an earlier part
         * is observed by the next send/final reply as a broken endpoint. */
        error=offset+count==description->bytes ?
            ntcon_worker_call(client,&request,&reply) : ntcon_worker_send(client,&request);
        offset+=count;
    }
    return error;
}

DWORD ntcon_worker_publish_title(ntcon_worker_client *client,const char *title)
{
    console_io_request request={0};console_io_reply reply;
    size_t length;
    if(!client || !title)return ERROR_INVALID_PARAMETER;
    length=strnlen_s(title,CONSOLE_IO_TITLE_BYTES);
    if(length==CONSOLE_IO_TITLE_BYTES)return ERROR_INVALID_PARAMETER;
    request.operation=CONSOLE_IO_PUBLISH_TITLE_A;
    request.bytes=(uint32_t)length+1;
    request.data=(BYTE *)title;
    return ntcon_worker_call(client,&request,&reply);
}

BOOL ntcon_worker_decode_input(const console_io_input *wire,INPUT_RECORD *record)
{
    if(!wire || !record)return FALSE;
    if(wire->type==CONSOLE_INPUT_FRAME_MOUSE) {
        console_frame_mouse_input mouse;
        if(wire->menu>UINT16_MAX || wire->focus>UINT16_MAX ||
            wire->control>UINT16_MAX || wire->buttons>UINT8_MAX || wire->flags>UINT8_MAX ||
            wire->repeat || wire->virtual_key || wire->scan || wire->character || wire->key_down)
            return FALSE;
        mouse.dx=wire->x;mouse.dy=wire->y;mouse.width=(uint16_t)wire->menu;
        mouse.height=(uint16_t)wire->focus;mouse.control=(uint16_t)wire->control;
        mouse.buttons=(uint8_t)wire->buttons;mouse.action=(uint8_t)wire->flags;
        if(!console_frame_mouse_input_valid(&mouse))return FALSE;
        ZeroMemory(record,sizeof(*record));record->EventType=CONSOLE_INPUT_FRAME_MOUSE;
        memcpy(&record->Event,&mouse,sizeof(mouse));return TRUE;
    }
    if(wire->type>UINT16_MAX || wire->repeat>UINT16_MAX || wire->virtual_key>UINT16_MAX ||
        wire->scan>UINT16_MAX || wire->character>UINT16_MAX || wire->key_down>1 || wire->focus>1 ||
        wire->x<SHRT_MIN || wire->x>SHRT_MAX || wire->y<SHRT_MIN || wire->y>SHRT_MAX)return FALSE;
    ZeroMemory(record,sizeof(*record));record->EventType=(WORD)wire->type;
    switch(wire->type) {
    case KEY_EVENT:
        record->Event.KeyEvent.bKeyDown=wire->key_down;
        record->Event.KeyEvent.wRepeatCount=(WORD)wire->repeat;
        record->Event.KeyEvent.wVirtualKeyCode=(WORD)wire->virtual_key;
        record->Event.KeyEvent.wVirtualScanCode=(WORD)wire->scan;
        record->Event.KeyEvent.uChar.UnicodeChar=(WCHAR)wire->character;
        record->Event.KeyEvent.dwControlKeyState=wire->control;break;
    case MOUSE_EVENT:
        record->Event.MouseEvent.dwMousePosition.X=(SHORT)wire->x;
        record->Event.MouseEvent.dwMousePosition.Y=(SHORT)wire->y;
        record->Event.MouseEvent.dwButtonState=wire->buttons;
        record->Event.MouseEvent.dwControlKeyState=wire->control;
        record->Event.MouseEvent.dwEventFlags=wire->flags;break;
    case WINDOW_BUFFER_SIZE_EVENT:
        record->Event.WindowBufferSizeEvent.dwSize.X=(SHORT)wire->x;
        record->Event.WindowBufferSizeEvent.dwSize.Y=(SHORT)wire->y;break;
    case MENU_EVENT:record->Event.MenuEvent.dwCommandId=wire->menu;break;
    case FOCUS_EVENT:record->Event.FocusEvent.bSetFocus=wire->focus;break;
    default:return FALSE;
    }
    return TRUE;
}
