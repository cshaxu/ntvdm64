/* Recovered from 286d54a3 presentation/text_producer.c: retain original
 * RegisterConsoleVDM cells and EGA metadata, replace worker UI/library frame
 * assembly with the admitted copied worker/frontend wire record. */
#include "console_text.h"
#include "console_client.h"
#include "product-abi/console_io.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_text_video.h"
#include <string.h>

BOOL NtvdmConsoleTextRequested(BOOL *requested)
{
    LONG values[4]={0};
    int result;
    if (!requested) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    *requested=FALSE;
    result=ntvdm_console_window_query(CONSOLE_WINDOW_TEXT_FRAME_REQUIRED,values);
    if (result<0) return TRUE; /* No character frontend, e.g. WOW. */
    if (!result) return GetLastError()==ERROR_NOT_READY; /* Native I/O owner. */
    *requested=values[0]!=0;
    return TRUE;
}

BOOL NtvdmConsoleUpdateText(HPALETTE palette)
{
    session *owner=session_thread_current();
    mvdm_softpc_text_video video;
    console_video_description description={0};
    console_text_style *style;
    PALETTEENTRY colours[16];
    uint32_t columns,rows,bytes,stride,index;
    BYTE *text=NULL,*payload=NULL;
    DWORD error;
    BOOL result=FALSE;
    if (!owner || !owner->console_client) return TRUE;
    if (!session_presentation_text_describe(owner,&columns,&rows,&bytes)) {
        SetLastError(ERROR_NOT_READY);return FALSE;
    }
    if (!mvdm_softpc_text_video_copy(&video)) return TRUE;
    if (!columns || !rows || columns>160 || rows>96 || !video.columns || !video.rows ||
        video.columns>columns || video.rows>rows || bytes%(columns*rows) ||
        bytes/(columns*rows)<2) { SetLastError(ERROR_INVALID_DATA);return FALSE; }
    if (!ntvdm_console_text_palette(colours)) {
        if (GetLastError()!=ERROR_NO_DATA) return FALSE;
        /* Only before the original VGA owner has published resolved colours. */
        if (GetPaletteEntries(palette,0,16,colours)!=16) {
            SetLastError(ERROR_INVALID_HANDLE);return FALSE;
        }
    }
    description.kind=CONSOLE_VIDEO_TEXT_FRAME;
    description.width=video.columns;description.height=video.rows;
    description.stride=video.columns*2;
    description.bytes=sizeof(*style)+description.stride*video.rows;
    payload=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,description.bytes);
    text=HeapAlloc(GetProcessHeap(),0,bytes);
    if (!payload || !text) { SetLastError(ERROR_NOT_ENOUGH_MEMORY);goto done; }
    if (!session_presentation_text_snapshot(owner,text,bytes,NULL,NULL,NULL)) {
        SetLastError(ERROR_INVALID_DATA);goto done;
    }
    style=(console_text_style *)payload;
    style->font_height=video.font_height;style->attribute_font_select=video.attribute_font_select!=0;
    style->cursor_column=video.cursor_column;style->cursor_row=video.cursor_row;
    style->cursor_start=video.cursor_start;style->cursor_height=video.cursor_height;
    style->cursor_start1=video.cursor_start1;style->cursor_height1=video.cursor_height1;
    style->cursor_visible=video.cursor_visible!=0;
    memcpy(style->fonts,video.fonts,sizeof(style->fonts));
    for (index=0;index<16;++index)
        description.palette[index]=((uint32_t)colours[index].peRed<<16) |
            ((uint32_t)colours[index].peGreen<<8) | colours[index].peBlue;
    stride=bytes/(columns*rows);
    for (index=0;index<video.columns*video.rows;++index) {
        uint32_t offset=((index/video.columns)*columns+index%video.columns)*stride;
        payload[sizeof(*style)+index*2]=text[offset];
        payload[sizeof(*style)+index*2+1]=text[offset+1];
    }
    result=ntvdm_console_publish_video(&description,payload,description.bytes);
    /* A queued graphics tick can finish after nt_block_event_thread has
     * handed I/O to a native child. The frontend rejects that stale paint
     * with NOT_READY; this is not loss of the frontend or task failure.
     * Do not retry into the child's surface. The next active refresh sends
     * a complete frame. All other publication failures remain errors. */
    if (!result && GetLastError()==ERROR_NOT_READY) result=TRUE;
done:
    error=GetLastError();
    if (text) HeapFree(GetProcessHeap(),0,text);
    if (payload) HeapFree(GetProcessHeap(),0,payload);
    SetLastError(error);return result;
}
