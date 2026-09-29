/* Project-owned worker client of the copied NTKVM protocol, shared by both backends.
 * Resource ownership and backend-specific handoff remain with each caller. */
#include "interface/worker_console_client.h"
#include <stddef.h>
#include <limits.h>
#include <string.h>

DWORD ntkvm_worker_client_init(ntkvm_worker_client *client,HANDLE pipe,HANDLE peer,HANDLE cancel,DWORD generation)
{
    if(!client || !pipe || pipe==INVALID_HANDLE_VALUE || !peer || !generation)
        return ERROR_INVALID_PARAMETER;
    client->event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!client->event)return GetLastError();
    client->pipe=pipe;client->peer=peer;client->cancel=cancel;client->generation=generation;
    return ERROR_SUCCESS;
}
void ntkvm_worker_client_dispose(ntkvm_worker_client *client)
{
    if(client->event)CloseHandle(client->event);
    ZeroMemory(client,sizeof(*client));
}

DWORD ntkvm_worker_activate(ntkvm_worker_client *client,DWORD kind,BOOL active)
{
    console_io_request request={0};console_io_reply reply;
    request.operation=CONSOLE_IO_DOS_ACTIVE;request.state.input=active!=FALSE;
    request.state.mode=kind;
    return ntkvm_worker_call(client,&request,&reply);
}

DWORD ntkvm_worker_prepend_keys(ntkvm_worker_client *client,const INPUT_RECORD *records,
    DWORD count,console_io_reply *reply)
{
    console_io_request request={0};DWORD i,error;
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
        memcpy(request.data+i*sizeof(wire),&wire,sizeof(wire));
    }
    error=ntkvm_worker_exchange(client,&request,reply);
    if(!error && reply->state.count>count)error=client->failure=ERROR_INVALID_DATA;
    return error;
}

static DWORD transfer(ntkvm_worker_client *client,BOOL write,void *buffer,DWORD bytes)
{
    BYTE *cursor=buffer;
    while(bytes) {
        OVERLAPPED io={0};
        HANDLE waits[3]={client->peer,client->event,client->cancel};
        DWORD count=0,error,wait;
        BOOL ok;
        if(client->cancel && WaitForSingleObject(client->cancel,0)==WAIT_OBJECT_0)
            return ERROR_OPERATION_ABORTED;
        if(WaitForSingleObject(client->peer,0)!=WAIT_TIMEOUT)return ERROR_PIPE_NOT_CONNECTED;
        ResetEvent(client->event);io.hEvent=client->event;
        ok=write ? WriteFile(client->pipe,cursor,bytes,&count,&io) :
            ReadFile(client->pipe,cursor,bytes,&count,&io);
        if(!ok) {
            error=GetLastError();if(error!=ERROR_IO_PENDING)return error;
            wait=WaitForMultipleObjects(client->cancel ? 3 : 2,waits,FALSE,INFINITE);
            if(wait!=WAIT_OBJECT_0+1) {
                error=wait==WAIT_FAILED ? GetLastError() :
                    wait==WAIT_OBJECT_0+2 ? ERROR_OPERATION_ABORTED : ERROR_PIPE_NOT_CONNECTED;
                CancelIoEx(client->pipe,&io);
                (void)GetOverlappedResult(client->pipe,&io,&count,TRUE);
                return error;
            }
            if(!GetOverlappedResult(client->pipe,&io,&count,FALSE))return GetLastError();
        }
        if(!count || count>bytes)return ERROR_BROKEN_PIPE;
        cursor+=count;bytes-=count;
    }
    return ERROR_SUCCESS;
}

DWORD ntkvm_worker_exchange(ntkvm_worker_client *client,console_io_request *request,console_io_reply *reply)
{
    DWORD error;
    if(!client || !request || !reply || request->bytes>CONSOLE_IO_DATA_BYTES)
        return ERROR_INVALID_PARAMETER;
    ZeroMemory(reply,sizeof(*reply));
    if(client->failure)return client->failure;
    if(client->sequence==UINT32_MAX)return ERROR_ARITHMETIC_OVERFLOW;
    request->version=CONSOLE_IO_VERSION;request->generation=client->generation;
    request->sequence=++client->sequence;
    error=transfer(client,TRUE,request,(DWORD)offsetof(console_io_request,data)+request->bytes);
    if(!error)error=transfer(client,FALSE,reply,(DWORD)offsetof(console_io_reply,data));
    if(!error && (reply->version!=CONSOLE_IO_VERSION || reply->generation!=client->generation ||
        reply->sequence!=client->sequence || reply->result>1 || (reply->result && reply->error) ||
        reply->bytes>CONSOLE_IO_DATA_BYTES ||
        (reply->bytes && request->operation!=CONSOLE_IO_GET_TITLE_A &&
            request->operation!=CONSOLE_IO_KEYBOARD_LAYOUT &&
            request->operation!=CONSOLE_IO_READ_TEXT_CONFIGURATION &&
            request->operation!=CONSOLE_IO_READ_INPUT && request->operation!=CONSOLE_IO_PEEK_INPUT &&
            (request->operation<CONSOLE_IO_READ_CELLS_A || request->operation>CONSOLE_IO_READ_CELLS_W))))
        error=ERROR_INVALID_DATA;
    if(!error)error=transfer(client,FALSE,reply->data,reply->bytes);
    if(error==ERROR_BROKEN_PIPE || error==ERROR_NO_DATA || error==ERROR_PIPE_NOT_CONNECTED)
        error=ERROR_PIPE_NOT_CONNECTED;
    if(error)client->failure=error;
    return error;
}

DWORD ntkvm_worker_call(ntkvm_worker_client *client,console_io_request *request,console_io_reply *reply)
{
    DWORD error=ntkvm_worker_exchange(client,request,reply);
    return error ? error : reply->result ? ERROR_SUCCESS : reply->error ? reply->error : ERROR_GEN_FAILURE;
}

DWORD ntkvm_worker_video(ntkvm_worker_client *client,const console_video_description *description,const void *pixels)
{
    console_io_request request={0};console_io_reply reply;
    DWORD error,offset=0,count;
    if(!client || (description && (!pixels || !description->bytes)))return ERROR_INVALID_PARAMETER;
    if(client->video_serial==UINT32_MAX)return ERROR_ARITHMETIC_OVERFLOW;
    request.state.mode=++client->video_serial;
    request.operation=description ? CONSOLE_IO_VIDEO_BEGIN : CONSOLE_IO_VIDEO_TEXT;
    if(description) {
        request.bytes=sizeof(*description);memcpy(request.data,description,sizeof(*description));
    }
    error=ntkvm_worker_call(client,&request,&reply);
    while(!error && description && offset<description->bytes) {
        count=min(description->bytes-offset,CONSOLE_IO_DATA_BYTES);
        request.operation=CONSOLE_IO_VIDEO_DATA;request.state.count=offset;request.bytes=count;
        memcpy(request.data,(const BYTE *)pixels+offset,count);
        error=ntkvm_worker_call(client,&request,&reply);offset+=count;
    }
    return error;
}

BOOL ntkvm_worker_decode_input(const console_io_input *wire,INPUT_RECORD *record)
{
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
