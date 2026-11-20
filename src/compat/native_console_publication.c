#include "compat/include/native_console_publication.h"
#include "compat/include/native_console_diagnostics.h"
#include <string.h>

DWORD ntvwm_presentation_exchange(ntvwm_presentation *client,console_io_request *request,console_io_reply *reply)
{
    DWORD error=ntcon_worker_call(&client->channel,request,reply);
    ntvwm_trace_error("exchange",request->operation,error);
    return error;
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

/* Entire transaction, including final video ACK, completes before the shared
 * engine records success. Never inspect mutable Console state on this thread. */
/* A VM with no copied font configuration still owns its real glyphs. Send
 * guest-codepage VGA cells through the existing transaction; never replace those
 * glyphs with the native worker's startup fallback bitmap. */
static DWORD publish_visible_cells(ntvwm_presentation *client,
    const native_publication *snapshot,const BYTE *payload)
{
    const console_video_description *d=&snapshot->description;
    const console_text_style *style=(const console_text_style *)payload;
    const BYTE *packed=payload+sizeof(*style);
    console_io_cell row[CONSOLE_IO_TILE_BYTES/sizeof(console_io_cell)];
    console_io_request q={0};console_io_reply reply;
    DWORD x,y,error,codepage,step=d->stride/d->width;
    CPINFO cp;
    const CHAR_INFO *cells=(const CHAR_INFO *)(snapshot+1);
    BOOL cursor_inside=style->cursor_column>=0 && style->cursor_row>=0 &&
        (DWORD)style->cursor_column<d->width && (DWORD)style->cursor_row<d->height;
    if(d->width>ARRAYSIZE(row))return ERROR_NOT_SUPPORTED;
    q.operation=CONSOLE_IO_CODE_PAGE;
    error=ntvwm_presentation_exchange(client,&q,&reply);if(error)return error;
    codepage=reply.state.count;
    if(codepage<=CP_THREAD_ACP)return ERROR_INVALID_DATA;
    if(!GetCPInfo(codepage,&cp))return GetLastError();
    if(cp.MaxCharSize!=1)return ERROR_NOT_SUPPORTED;
    for(y=0;y<d->height;++y) {
        for(x=0;x<d->width;++x) {

            const BYTE *p=packed+y*d->stride+x*step;
            if(step==3 && p[2])return ERROR_NOT_SUPPORTED;
            row[x].character=p[0];row[x].attribute=p[1];
            if(codepage!=437) {
                const CHAR_INFO *source=cells+(y+snapshot->info.srWindow.Top)*snapshot->info.dwSize.X+
                    snapshot->info.srWindow.Left+x;
                /* Keep the packer's trailing/wide-cell blank, but map actual
                 * characters using the guest owner rather than the NTCON font. */
                if(!(source->Attributes&COMMON_LVB_TRAILING_BYTE) &&
                    !((x || snapshot->info.srWindow.Left) && source->Char.UnicodeChar>=0xdc00 &&
                      source->Char.UnicodeChar<=0xdfff && source[-1].Char.UnicodeChar>=0xd800 &&
                      source[-1].Char.UnicodeChar<=0xdbff))
                    row[x].character=ntvwm_text_glyph(codepage,source->Char.UnicodeChar);
            }
        }
        ZeroMemory(&q,sizeof(q));q.operation=CONSOLE_IO_WRITE_CELLS_A;
        q.state.width=d->width;q.state.height=1;q.state.right=d->width-1;
        q.state.top=q.state.bottom=y;q.bytes=d->width*sizeof(*row);q.data=(BYTE *)row;
        error=ntvwm_presentation_exchange(client,&q,&reply);if(error)return error;
    }
    /* The hidden Console retains the absolute cursor even outside its visible
     * viewport. The guest projection hides it until it enters that viewport. */
    if(cursor_inside) {
        ZeroMemory(&q,sizeof(q));q.operation=CONSOLE_IO_CURSOR_POSITION;
        q.state.x=style->cursor_column;q.state.y=style->cursor_row;
        error=ntvwm_presentation_exchange(client,&q,&reply);if(error)return error;
    }
    ZeroMemory(&q,sizeof(q));q.operation=CONSOLE_IO_CURSOR_INFO;
    q.state.cursor_size=snapshot->cursor.dwSize;
    q.state.cursor_visible=cursor_inside && snapshot->cursor.bVisible!=FALSE;
    return ntvwm_presentation_exchange(client,&q,&reply);
}

void ntvwm_publication_set_cells(ntvwm_presentation *client, CHAR_INFO *cells,
    DWORD count, DWORD width)
{
    if(client->published_cells)HeapFree(GetProcessHeap(),0,client->published_cells);
    client->published_cells=cells;
    client->published_count=count;
    client->published_width=width;
}

DWORD ntvwm_publish_copy(void *context,const void *copy,SIZE_T bytes)
{
    ntvwm_presentation *client=context;
    const native_publication *snapshot=copy;
    const CHAR_INFO *cells;
    const BYTE *payload;
    console_video_description description;
    console_io_request request={0};console_io_reply reply;
    DWORD error=0,total,offset=0,count;
    BOOL publication_held=FALSE,frame_only;
    CHAR_INFO *published;
    SIZE_T cell_bytes;
    if(bytes<sizeof(*snapshot))return ERROR_INVALID_DATA;
    total=snapshot->total;cell_bytes=(SIZE_T)total*sizeof(*cells);
    if(!total || total>SIZE_MAX/sizeof(*cells) ||
        cell_bytes>bytes-sizeof(*snapshot) ||
        bytes-sizeof(*snapshot)-cell_bytes!=snapshot->description.bytes)
        return ERROR_INVALID_DATA;
    cells=(const CHAR_INFO *)(snapshot+1);
    payload=(const BYTE *)cells+cell_bytes;description=snapshot->description;
    published=HeapAlloc(GetProcessHeap(),0,cell_bytes);
    if(!published)return ERROR_NOT_ENOUGH_MEMORY;
    memcpy(published,cells,cell_bytes);
    EnterCriticalSection(&client->lock);
    if(snapshot->title_read && (!client->title_valid ||
        strcmp(client->published_title,snapshot->title))) {
        error=ntcon_worker_publish_title(&client->channel,snapshot->title);
        if(!error) {
            strcpy_s(client->published_title,sizeof(client->published_title),snapshot->title);
            client->title_valid=TRUE;
        }
    }
    if(error)goto captured_done;
    ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_WINDOW_QUERY;
    request.state.mode=CONSOLE_WINDOW_PRESENTATION_ONLY;
    error=ntvwm_presentation_exchange(client,&request,&reply);
    if(error)goto captured_done;
    frame_only=reply.state.left!=FALSE;
    ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_PUBLICATION_BEGIN;
    error=ntvwm_presentation_exchange(client,&request,&reply);
    if(error)goto captured_done;
    publication_held=TRUE;
    /* A VM consumes the already packed visible frame. Native scrollback and
     * its absolute cursor/viewport remain owned by the hidden Console. */
    if(frame_only) {
        ZeroMemory(&request,sizeof(request));
        request.operation=CONSOLE_IO_PREPARE_TEXT_REGION;
        request.state.width=description.width;
        request.state.height=description.height;
        error=ntvwm_presentation_exchange(client,&request,&reply);
        if(error)goto captured_done;
    }
    if(!frame_only) {
        request.operation=CONSOLE_IO_SCREEN_INFO;
        error=ntvwm_presentation_exchange(client,&request,&reply);
        if(!error && (reply.state.width!=snapshot->info.dwSize.X || reply.state.height!=snapshot->info.dwSize.Y)) {
            /* Same order as ntvwm_screen_apply: grow before moving the viewport,
             * shrink only after it fits. A native TUI may shrink 120x9001 to
             * 80x25; shrinking beneath the old visible window is invalid. */
            LONG width=max(reply.state.width,snapshot->info.dwSize.X);
            LONG height=max(reply.state.height,snapshot->info.dwSize.Y);
            if(width!=reply.state.width || height!=reply.state.height) {
                request.operation=CONSOLE_IO_BUFFER_SIZE;
                request.state.width=width;request.state.height=height;
                error=ntvwm_presentation_exchange(client,&request,&reply);
            }
            if(!error) {
                ZeroMemory(&request,sizeof(request));
                request.operation=CONSOLE_IO_WINDOW_RECT;request.state.mode=1;
                request.state.left=snapshot->info.srWindow.Left;request.state.right=snapshot->info.srWindow.Right;
                request.state.top=snapshot->info.srWindow.Top;request.state.bottom=snapshot->info.srWindow.Bottom;
                error=ntvwm_presentation_exchange(client,&request,&reply);
            }
            if(!error && (width!=snapshot->info.dwSize.X || height!=snapshot->info.dwSize.Y)) {
                request.operation=CONSOLE_IO_BUFFER_SIZE;
                request.state.width=snapshot->info.dwSize.X;request.state.height=snapshot->info.dwSize.Y;
                error=ntvwm_presentation_exchange(client,&request,&reply);
            }
        }
        for(offset=0;!error && offset<total;offset+=count) {
            ZeroMemory(&request,sizeof(request));
            count=min((DWORD)snapshot->info.dwSize.X-offset%snapshot->info.dwSize.X,
                CONSOLE_IO_TILE_BYTES/sizeof(console_io_cell));
            /* A sampled cursor/font change is not a write to every Console row.
             * Derive dirty rows at this native producer, like VGA calc_update;
             * explicit exchange/text publications remain completely unfiltered. */
            if(client->published_count==total && client->published_width==(DWORD)snapshot->info.dwSize.X &&
                !memcmp(client->published_cells+offset,cells+offset,count*sizeof(*cells)))continue;
            request.operation=CONSOLE_IO_WRITE_CELLS_W;
            request.state.width=count;request.state.height=1;
            request.state.left=offset%snapshot->info.dwSize.X;request.state.right=request.state.left+count-1;
            request.state.top=request.state.bottom=offset/snapshot->info.dwSize.X;
            request.bytes=count*sizeof(console_io_cell);
            request.data=(BYTE *)(cells+offset);
            error=ntvwm_presentation_exchange(client,&request,&reply);
        }
        if(!error) {
            ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_CURSOR_POSITION;
            request.state.x=snapshot->info.dwCursorPosition.X;request.state.y=snapshot->info.dwCursorPosition.Y;
            error=ntvwm_presentation_exchange(client,&request,&reply);
        }
        if(!error) {
            ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_WINDOW_RECT;request.state.mode=1;
            request.state.left=snapshot->info.srWindow.Left;request.state.right=snapshot->info.srWindow.Right;
            request.state.top=snapshot->info.srWindow.Top;request.state.bottom=snapshot->info.srWindow.Bottom;
            error=ntvwm_presentation_exchange(client,&request,&reply);
        }
        if(!error) {
            ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_CURSOR_INFO;
            request.state.cursor_size=snapshot->cursor.dwSize;request.state.cursor_visible=snapshot->cursor.bVisible!=FALSE;
            error=ntvwm_presentation_exchange(client,&request,&reply);
        }
    }
    if(!error) {
        ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_ATTRIBUTE;
        request.state.attribute=snapshot->info.wAttributes;
        error=ntvwm_presentation_exchange(client,&request,&reply);
    }
    if(!error) {
        if(frame_only && !client->has_handoff_font)
            error=publish_visible_cells(client,snapshot,payload);
        else error=ntvwm_presentation_text(client,&description,payload,description.bytes);
    }
    if(!error) {
        ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_PUBLICATION_END;
        error=ntvwm_presentation_exchange(client,&request,&reply);
        if(!error)publication_held=FALSE;
    }
    if(error)ntvwm_publication_set_cells(client,NULL,0,0);
    else {
        ntvwm_publication_set_cells(client,published,total,(DWORD)snapshot->info.dwSize.X);
        published=NULL;
    }
captured_done:
    if(publication_held) {
        DWORD aborted;
        ZeroMemory(&request,sizeof(request));request.operation=CONSOLE_IO_PUBLICATION_ABORT;
        aborted=ntvwm_presentation_exchange(client,&request,&reply);if(!error)error=aborted;
    }
    LeaveCriticalSection(&client->lock);
    if(published)HeapFree(GetProcessHeap(),0,published);
    return error;
}
