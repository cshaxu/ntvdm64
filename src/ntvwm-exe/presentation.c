/* Same request ordering/frame protocol as ntvdm-exe/win32/console_client.c.
 * No guest context, original Console-close policy or frontend ownership here. */
#include "presentation.h"
#include "common/console/client.h"
#include "console_state.h"
#include "text_frame.h"
#include <stddef.h>
#include <string.h>
#include <limits.h>
struct ntvwm_presentation {
    ntcon_worker_client channel;
    CRITICAL_SECTION lock;
    CHAR_INFO *published_cells;
    DWORD published_count,published_width;
    console_text_style handoff_font;
    BOOL has_handoff_font;
    char published_title[CONSOLE_IO_TITLE_BYTES];
    BOOL title_valid;
    ntvwm_mouse mouse;
};
static DWORD exchange(ntvwm_presentation *client,console_io_request *request,console_io_reply *reply)
{
    DWORD error=ntcon_worker_call(&client->channel,request,reply);
    ntvwm_trace_error("exchange",request->operation,error);
    return error;
}
DWORD ntvwm_presentation_open(HANDLE pipe,HANDLE frontend,HANDLE stop,DWORD generation,
    ntvwm_presentation **output)
{
    ntvwm_presentation *client;DWORD error;
    if(!output)return ERROR_INVALID_PARAMETER;
    *output=NULL;
    if(!pipe || pipe==INVALID_HANDLE_VALUE || !frontend || !generation)return ERROR_INVALID_PARAMETER;
    client=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*client));
    if(!client)return ERROR_NOT_ENOUGH_MEMORY;
    error=ntcon_worker_client_init(&client->channel,pipe,frontend,stop,generation);
    if(error) { HeapFree(GetProcessHeap(),0,client);return error; }
    InitializeCriticalSection(&client->lock);*output=client;return ERROR_SUCCESS;
}
void ntvwm_presentation_close(ntvwm_presentation *client)
{
    if(!client)return;
    if(client->published_cells)HeapFree(GetProcessHeap(),0,client->published_cells);
    ntcon_worker_client_dispose(&client->channel);DeleteCriticalSection(&client->lock);
    HeapFree(GetProcessHeap(),0,client);
}
DWORD ntvwm_presentation_call(ntvwm_presentation *client,const console_io_request *input,
    console_io_reply *reply)
{
    console_io_request request;DWORD error;
    if(!client || !input || !reply || input->bytes>CONSOLE_IO_DATA_BYTES ||
        input->operation<CONSOLE_IO_WRITE || input->operation>CONSOLE_IO_PREPARE_TEXT_REGION)
        return ERROR_INVALID_PARAMETER;
    memcpy(&request,input,offsetof(console_io_request,data)+input->bytes);
    EnterCriticalSection(&client->lock);error=exchange(client,&request,reply);
    LeaveCriticalSection(&client->lock);return error;
}
DWORD ntvwm_presentation_input(ntvwm_presentation *client,HANDLE input,DWORD *accepted)
{
    console_io_request request={0};console_io_reply reply;
    INPUT_RECORD records[CONSOLE_IO_INPUT_CAPACITY*2];DWORD error,index,count=0;
    if(!client || !accepted || !input || input==INVALID_HANDLE_VALUE)return ERROR_INVALID_PARAMETER;
    *accepted=0;
    request.operation=CONSOLE_IO_READ_INPUT;request.state.count=CONSOLE_IO_INPUT_CAPACITY;
    EnterCriticalSection(&client->lock);
    error=exchange(client,&request,&reply);
    if(!error && (reply.state.count>CONSOLE_IO_INPUT_CAPACITY ||
        reply.bytes!=reply.state.count*sizeof(console_io_input)))error=ERROR_INVALID_DATA;
    if(!error && reply.state.count) {
        ntvwm_capture capture={0};
        error=ntvwm_capture_begin(&capture);
        if(!error)error=ntvwm_mouse_geometry(&client->mouse,capture.info.srWindow,
            client->has_handoff_font ? client->handoff_font.font_height : 16);
        ntvwm_capture_end(&capture);
    }
    for(index=0;!error && index<reply.state.count;++index) {
        console_io_input wire;
        memcpy(&wire,reply.data+index*sizeof(wire),sizeof(wire));
        if(wire.type==CONSOLE_INPUT_FRAME_MOUSE) {
            console_frame_mouse_input frame;DWORD generated=0;
            INPUT_RECORD decoded;
            if(!ntcon_worker_decode_input(&wire,&decoded)) {
                error=ERROR_INVALID_DATA;break;
            }
            memcpy(&frame,&decoded.Event,sizeof(frame));
            error=ntvwm_mouse_input(&client->mouse,&frame,records+count,&generated);
            count+=generated;
        } else {
            if(!ntcon_worker_decode_input(&wire,&records[count]))error=ERROR_INVALID_DATA;
            else {
                if(wire.type==MOUSE_EVENT && client->mouse.ready) {
                    ntvwm_mouse *mouse=&client->mouse;
                    mouse->x=max(0,min(wire.x-mouse->viewport.Left,
                        mouse->viewport.Right-mouse->viewport.Left))*8;
                    mouse->y=max(0,min(wire.y-mouse->viewport.Top,
                        mouse->viewport.Bottom-mouse->viewport.Top))*mouse->font_height;
                    mouse->buttons=wire.buttons&0xffff;mouse->visible=FALSE;
                }
                ++count;
            }
        }
    }
    if(!error && count)error=ntvwm_input_write(input,records,count,accepted);
    /* Once removed from the frontend queue, an ambiguous failed batch cannot
     * be retried. Retain the failure rather than duplicate delivered keys. */
    if(error && error!=ERROR_NOT_READY && error!=ERROR_BUSY)client->channel.failure=error;
    LeaveCriticalSection(&client->lock);return error;
}
DWORD ntvwm_presentation_text(ntvwm_presentation *client,const console_video_description *description,
    const void *payload,SIZE_T capacity)
{
    DWORD error;
    /* NTVWM cannot send DIBs or an inconsistent text shape. Geometry limits
     * and style validation remain with the unchanged common receiver. */
    if(!client || !description || !payload || description->kind!=CONSOLE_VIDEO_TEXT_FRAME ||
        description->depth || !description->width || !description->height ||
        description->width>UINT32_MAX/3 ||
        (description->stride!=description->width*2 && description->stride!=description->width*3) ||
        (uint64_t)sizeof(console_text_style)+(uint64_t)description->stride*description->height!=description->bytes ||
        capacity<description->bytes)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&client->lock);
    error=ntcon_worker_video(&client->channel,description,payload);
    LeaveCriticalSection(&client->lock);return error;
}

DWORD ntvwm_presentation_capture(ntvwm_presentation *client,const console_text_style *font)
{
    ntvwm_capture capture={0};CHAR_INFO *cells=NULL;BYTE *payload=NULL;
    char title[CONSOLE_IO_TITLE_BYTES]={0};BOOL title_read=FALSE,publication_held=FALSE;
    console_io_request request={0};console_io_reply reply;
    console_video_description description={0};DWORD error,total,offset=0,count;
    SMALL_RECT region;
    if(!client || !font)return ERROR_INVALID_PARAMETER;
    error=ntvwm_capture_begin(&capture);
    if(error)return error;
    SetLastError(ERROR_SUCCESS);
    if(GetConsoleTitleA(title,sizeof(title)) || GetLastError()==ERROR_SUCCESS) {
        title[sizeof(title)-1]=0;title_read=TRUE;
    }
    total=(DWORD)capture.info.dwSize.X*(DWORD)capture.info.dwSize.Y;
    if(!total || total>SIZE_MAX/sizeof(*cells)) { error=ERROR_ARITHMETIC_OVERFLOW;goto done; }
    cells=HeapAlloc(GetProcessHeap(),0,(SIZE_T)total*sizeof(*cells));
    if(!cells) { error=ERROR_NOT_ENOUGH_MEMORY;goto done; }
    while(offset<total) {
        DWORD capacity=total-offset;
        if(capacity>4096)capacity=4096;
        error=ntvwm_capture_read(&capture,offset,cells+offset,capacity,&region,&count);
        if(error)goto done;
        if(!count || count>capacity) { error=ERROR_INVALID_DATA;goto done; }
        offset+=count;
    }
    EnterCriticalSection(&client->lock);
    error=ntvwm_mouse_geometry(&client->mouse,capture.info.srWindow,
        client->has_handoff_font ? client->handoff_font.font_height : font->font_height);
    if(!error)error=ntvwm_text_frame_pack(&capture.info,&capture.cursor,cells,total,
        client->has_handoff_font ? &client->handoff_font : font,&description,&payload);
    if(!error)ntvwm_mouse_compose(&client->mouse,&description,payload);
    LeaveCriticalSection(&client->lock);
    if(error)goto done;
    /* Publish the complete logical cell grid and its viewport text metadata
     * as one transaction. Unicode remains in logical Console cells; bounded
     * PC glyph conversion remains in this worker, not the frontend. */
    EnterCriticalSection(&client->lock);
    if(title_read && (!client->title_valid || strcmp(client->published_title,title))) {
        error=ntcon_worker_publish_title(&client->channel,title);
        if(!error) {
            strcpy_s(client->published_title,sizeof(client->published_title),title);
            client->title_valid=TRUE;
        }
    }
    if(error==ERROR_NOT_READY || error==ERROR_BUSY)error=ERROR_SUCCESS;
    if(error)goto captured_done;
    ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_PUBLICATION_BEGIN;
    error=exchange(client,&request,&reply);
    if(error)goto captured_done;
    publication_held=TRUE;
    request.operation=CONSOLE_IO_SCREEN_INFO;
    error=exchange(client,&request,&reply);
    if(!error && (reply.state.width!=capture.info.dwSize.X || reply.state.height!=capture.info.dwSize.Y)) {
        /* Same order as ntvwm_screen_apply: grow before moving the viewport,
         * shrink only after it fits. A native TUI may shrink 120x9001 to
         * 80x25; shrinking beneath the old visible window is invalid. */
        LONG width=max(reply.state.width,capture.info.dwSize.X);
        LONG height=max(reply.state.height,capture.info.dwSize.Y);
        if(width!=reply.state.width || height!=reply.state.height) {
            request.operation=CONSOLE_IO_BUFFER_SIZE;
            request.state.width=width;request.state.height=height;
            error=exchange(client,&request,&reply);
        }
        if(!error) {
            ZeroMemory(&request,sizeof(request));
            request.operation=CONSOLE_IO_WINDOW_RECT;request.state.mode=1;
            request.state.left=capture.info.srWindow.Left;request.state.right=capture.info.srWindow.Right;
            request.state.top=capture.info.srWindow.Top;request.state.bottom=capture.info.srWindow.Bottom;
            error=exchange(client,&request,&reply);
        }
        if(!error && (width!=capture.info.dwSize.X || height!=capture.info.dwSize.Y)) {
            request.operation=CONSOLE_IO_BUFFER_SIZE;
            request.state.width=capture.info.dwSize.X;request.state.height=capture.info.dwSize.Y;
            error=exchange(client,&request,&reply);
        }
    }
    for(offset=0;!error && offset<total;offset+=count) {
        ZeroMemory(&request,sizeof(request));
        count=min((DWORD)capture.info.dwSize.X-offset%capture.info.dwSize.X,
            CONSOLE_IO_DATA_BYTES/sizeof(console_io_cell));
        if(client->published_count==total && client->published_width==(DWORD)capture.info.dwSize.X &&
            !memcmp(client->published_cells+offset,cells+offset,count*sizeof(*cells)))continue;
        request.operation=CONSOLE_IO_WRITE_CELLS_W;
        request.state.width=count;request.state.height=1;
        request.state.left=offset%capture.info.dwSize.X;request.state.right=request.state.left+count-1;
        request.state.top=request.state.bottom=offset/capture.info.dwSize.X;
        request.bytes=count*sizeof(console_io_cell);
        memcpy(request.data,cells+offset,request.bytes);
        error=exchange(client,&request,&reply);
    }
    if(!error) {
        ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_CURSOR_POSITION;
        request.state.x=capture.info.dwCursorPosition.X;request.state.y=capture.info.dwCursorPosition.Y;
        error=exchange(client,&request,&reply);
    }
    if(!error) {
        ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_WINDOW_RECT;request.state.mode=1;
        request.state.left=capture.info.srWindow.Left;request.state.right=capture.info.srWindow.Right;
        request.state.top=capture.info.srWindow.Top;request.state.bottom=capture.info.srWindow.Bottom;
        error=exchange(client,&request,&reply);
    }
    if(!error) {
        ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_CURSOR_INFO;
        request.state.cursor_size=capture.cursor.dwSize;request.state.cursor_visible=capture.cursor.bVisible!=FALSE;
        error=exchange(client,&request,&reply);
    }
    if(!error) {
        ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_ATTRIBUTE;
        request.state.attribute=capture.info.wAttributes;
        error=exchange(client,&request,&reply);
    }
    if(!error)error=ntvwm_presentation_text(client,&description,payload,description.bytes);
    if(!error) {
        ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_PUBLICATION_END;
        error=exchange(client,&request,&reply);
        if(!error)publication_held=FALSE;
    }
    if(client->published_cells)HeapFree(GetProcessHeap(),0,client->published_cells);
    client->published_cells=NULL;client->published_count=0;
    if(!error) {
        client->published_cells=cells;cells=NULL;client->published_count=total;
        client->published_width=(DWORD)capture.info.dwSize.X;
    }
captured_done:
    if(publication_held) {
        DWORD aborted;
        ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_PUBLICATION_ABORT;
        aborted=exchange(client,&request,&reply);if(!error)error=aborted;
    }
    LeaveCriticalSection(&client->lock);
done:
    if(payload)HeapFree(GetProcessHeap(),0,payload);
    if(cells)HeapFree(GetProcessHeap(),0,cells);
    ntvwm_trace_error("capture",0,error);
    ntvwm_capture_end(&capture);return error;
}

static DWORD read_configuration(ntvwm_presentation *client,console_text_configuration *configuration,BOOL *found)
{
    console_io_request request={0};console_io_reply reply;
    DWORD offset=0,revision=0,error;
    *found=FALSE;
    while(offset<sizeof(*configuration)) {
        request.operation=CONSOLE_IO_READ_TEXT_CONFIGURATION;
        request.state.count=offset;request.state.mode=revision;
        error=exchange(client,&request,&reply);
        if(error==ERROR_NOT_FOUND && !offset)return ERROR_SUCCESS;
        if(error)return error;
        if(!reply.state.mode || (revision && revision!=reply.state.mode) ||
            reply.state.count!=sizeof(*configuration) ||
            reply.bytes!=min(sizeof(*configuration)-offset,CONSOLE_IO_DATA_BYTES))return ERROR_INVALID_DATA;
        revision=reply.state.mode;
        memcpy((BYTE *)configuration+offset,reply.data,reply.bytes);offset+=reply.bytes;
    }
    if(!configuration->style.font_height || configuration->style.font_height>32 ||
        configuration->style.attribute_font_select>1)return ERROR_INVALID_DATA;
    *found=TRUE;return ERROR_SUCCESS;
}
DWORD ntvwm_presentation_seed(ntvwm_presentation *client,HANDLE output)
{
    console_io_request request={0};console_io_reply reply;
    console_io_state screen;
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)};
    CONSOLE_CURSOR_INFO cursor;
    console_text_configuration configuration;
    BOOL has_configuration=FALSE,snapshot_held=FALSE;
    CHAR_INFO *cells=NULL;
    DWORD error,total,offset=0,width,count,index;
    if(!client || !output || output==INVALID_HANDLE_VALUE)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&client->lock);
    request.operation=CONSOLE_IO_SNAPSHOT_BEGIN;
    error=exchange(client,&request,&reply);
    if(error)goto done;
    snapshot_held=TRUE;
    request.operation=CONSOLE_IO_SCREEN_INFO;
    error=exchange(client,&request,&reply);
    if(error)goto done;
    screen=reply.state;
    if(screen.width<=0 || screen.width>SHRT_MAX || screen.height<=0 || screen.height>SHRT_MAX ||
        screen.x<0 || screen.x>=screen.width || screen.y<0 || screen.y>=screen.height ||
        screen.left<0 || screen.top<0 || screen.right<screen.left || screen.bottom<screen.top ||
        screen.right>=screen.width || screen.bottom>=screen.height || screen.attribute>UINT16_MAX) {
        error=ERROR_INVALID_DATA;goto done;
    }
    width=(DWORD)screen.width;total=width*(DWORD)screen.height;
    if(total>SIZE_MAX/sizeof(*cells)) {error=ERROR_ARITHMETIC_OVERFLOW;goto done;}
    cells=HeapAlloc(GetProcessHeap(),0,(SIZE_T)total*sizeof(*cells));
    if(!cells) {error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    request.operation=CONSOLE_IO_GET_CURSOR_INFO;
    error=exchange(client,&request,&reply);
    if(error)goto done;
    if(!reply.state.cursor_size || reply.state.cursor_size>100 || reply.state.cursor_visible>1) {
        error=ERROR_INVALID_DATA;goto done;
    }
    cursor.dwSize=reply.state.cursor_size;cursor.bVisible=reply.state.cursor_visible;
    while(offset<total) {
        DWORD columns=min(width-offset%width,CONSOLE_IO_DATA_BYTES/sizeof(console_io_cell));
        DWORD rows=offset%width || columns!=width ? 1 :
            min((total-offset)/width,(CONSOLE_IO_DATA_BYTES/sizeof(console_io_cell))/width);
        count=columns*rows;
        request.operation=CONSOLE_IO_READ_CELLS_W;
        request.state.width=columns;request.state.height=rows;
        request.state.left=offset%width;request.state.right=request.state.left+columns-1;
        request.state.top=offset/width;request.state.bottom=request.state.top+rows-1;
        error=exchange(client,&request,&reply);
        if(error)goto done;
        if(reply.bytes!=count*sizeof(console_io_cell) || reply.state.left!=request.state.left ||
            reply.state.right!=request.state.right || reply.state.top!=request.state.top ||
            reply.state.bottom!=request.state.bottom) {error=ERROR_RETRY;goto done;}
        for(index=0;index<count;++index) {
            console_io_cell cell;
            memcpy(&cell,reply.data+index*sizeof(cell),sizeof(cell));
            cells[offset+index].Char.UnicodeChar=cell.character;
            cells[offset+index].Attributes=cell.attribute;
        }
        offset+=count;
    }
    ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_SCREEN_INFO;
    error=exchange(client,&request,&reply);
    if(error)goto done;
    if(memcmp(&screen,&reply.state,sizeof(screen))) {error=ERROR_RETRY;goto done;}
    error=read_configuration(client,&configuration,&has_configuration);
    if(error)goto done;
    ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_SNAPSHOT_END;
    error=exchange(client,&request,&reply);
    snapshot_held=FALSE;
    if(error)goto done;
    if(!GetConsoleScreenBufferInfoEx(output,&info)) {error=GetLastError();goto done;}
    /* The acknowledged logical grid is the entire current handoff state.
     * Neither old native storage nor history matching may bias its origin. */
    if(has_configuration)for(index=0;index<16;++index) {
        DWORD rgb=configuration.palette[index];
        if(rgb>0xffffff) {error=ERROR_INVALID_DATA;goto done;}
        info.ColorTable[index]=RGB((rgb>>16)&255,(rgb>>8)&255,rgb&255);
    }
    info.dwSize.X=(SHORT)screen.width;info.dwSize.Y=(SHORT)screen.height;
    info.dwCursorPosition.X=(SHORT)screen.x;info.dwCursorPosition.Y=(SHORT)screen.y;
    info.srWindow.Left=(SHORT)screen.left;info.srWindow.Top=(SHORT)screen.top;
    info.srWindow.Right=(SHORT)screen.right;info.srWindow.Bottom=(SHORT)screen.bottom;
    info.wAttributes=(WORD)screen.attribute;
    error=ntvwm_screen_apply(output,&info,&cursor);
    for(offset=0;!error && offset<total;offset+=count) {
        count=min(width-offset%width,CONSOLE_IO_DATA_BYTES/sizeof(console_io_cell));
        if(!(offset%width) && count==width)
            count=width*min((total-offset)/width,(CONSOLE_IO_DATA_BYTES/sizeof(console_io_cell))/width);
        error=ntvwm_cells_write(output,offset,cells+offset,count);
    }
    /* Seed is the acknowledged common screen. Publish subsequent changes,
     * not thousands of unchanged scrollback rows on every input iteration. */
    if(!error) {
        client->has_handoff_font=has_configuration;
        if(has_configuration)client->handoff_font=configuration.style;
        if(client->published_cells)HeapFree(GetProcessHeap(),0,client->published_cells);
        client->published_cells=NULL;client->published_count=0;
        client->published_cells=cells;cells=NULL;
        client->published_count=total;client->published_width=width;
        client->mouse.visible=FALSE;client->mouse.buttons=0;
        error=ntvwm_mouse_geometry(&client->mouse,info.srWindow,
            has_configuration ? configuration.style.font_height : 16);
    }
done:
    if(snapshot_held) {
        DWORD release;
        ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_SNAPSHOT_END;
        release=exchange(client,&request,&reply);
        if(!error)error=release;
    }
    if(cells)HeapFree(GetProcessHeap(),0,cells);
    LeaveCriticalSection(&client->lock);return error;
}

static DWORD activate_presentation(ntvwm_presentation *client,BOOL active)
{
    return ntcon_worker_activate(&client->channel,active);
}
DWORD ntvwm_presentation_begin(ntvwm_presentation *client,HANDLE output)
{
    DWORD error;
    if(!client || !output || output==INVALID_HANDLE_VALUE)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&client->lock);
    error=activate_presentation(client,TRUE);
    if(!error) {
        error=ntvwm_presentation_seed(client,output);
        if(!error) {
            console_io_request request={0};console_io_reply reply;
            request.operation=CONSOLE_IO_SET_MODE;request.state.input=1;
            request.state.mode=ENABLE_WINDOW_INPUT|ENABLE_MOUSE_INPUT|ENABLE_EXTENDED_FLAGS;
            error=exchange(client,&request,&reply);
        }
        if(error)(void)activate_presentation(client,FALSE);
    }
    LeaveCriticalSection(&client->lock);return error;
}
/* Return only records still present in an empty native Console, never replay
 * consumed input. Same PREPEND_KEYS wire shape as console_client.c. The caller
 * serializes execution admission and has stopped this endpoint's input pump. */
static DWORD return_unused_input(ntvwm_presentation *client)
{
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE);
    DWORD pending=0,read=0,error=0,index,keys=0;
    INPUT_RECORD *records=NULL;
    /* Input ownership is transferred by the direct command's handoff.
     * A Console process snapshot cannot establish task membership. */
    if(!GetNumberOfConsoleInputEvents(input,&pending))return GetLastError();
    if(!pending)return 0;
    if(pending>SIZE_MAX/sizeof(*records))return ERROR_ARITHMETIC_OVERFLOW;
    records=HeapAlloc(GetProcessHeap(),0,pending*sizeof(*records));
    if(!records)return ERROR_NOT_ENOUGH_MEMORY;
    if(!ReadConsoleInputW(input,records,pending,&read))error=GetLastError();
    for(index=0;!error && index<read;++index)
        if(records[index].EventType==KEY_EVENT)records[keys++]=records[index];
    /* Prepend batches from the tail so the resulting queue remains FIFO. */
    while(!error && keys) {
        console_io_reply reply;
        DWORD count=min(keys,CONSOLE_IO_INPUT_CAPACITY),base=keys-count;
        error=ntcon_worker_prepend_keys(&client->channel,records+base,count,&reply);
        if(!error && !reply.result)error=reply.error ? reply.error : ERROR_GEN_FAILURE;
        if(!error && reply.state.count!=count)error=ERROR_WRITE_FAULT;
        keys=base;
    }
    HeapFree(GetProcessHeap(),0,records);return error;
}
DWORD ntvwm_presentation_end(ntvwm_presentation *client,const console_text_style *font)
{
    console_io_request request={0};console_io_reply reply;DWORD error,released,attempt;
    if(!client || !font)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&client->lock);
    {
        console_frame_mouse_input leave={0};INPUT_RECORD records[2];DWORD count=0,written=0;
        leave.action=CONSOLE_MOUSE_LEAVE;
        error=client->mouse.ready ? ntvwm_mouse_input(&client->mouse,&leave,records,&count) : 0;
        if(!error && count)error=ntvwm_input_write(GetStdHandle(STD_INPUT_HANDLE),records,count,&written);
    }
    /* A native target may resize during capture. Restart from a fresh
     * snapshot before returning unused input or releasing ownership. */
    if(!error)for(attempt=0;attempt<8;++attempt) {
        error=ntvwm_presentation_capture(client,font);
        if(error!=ERROR_RETRY)break;
        if(attempt<7)Sleep(10);
    }
    if(!error)error=return_unused_input(client);
    if(!error) {
        request.operation=CONSOLE_IO_BARRIER;
        error=exchange(client,&request,&reply);
    }
    released=activate_presentation(client,FALSE);
    client->title_valid=FALSE;
    LeaveCriticalSection(&client->lock);return error ? error : released;
}
