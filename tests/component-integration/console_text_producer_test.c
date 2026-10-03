/* Production frame packer with explicit synthetic original-state boundaries.
 * The reader and actual IPC have separate tests; this is not guest proof. */
#include "ntvdm-exe/win32/console_text.h"
#include "ntvdm-exe/win32/console_client.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_text_video.h"
#include "common/protocol/console_io.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d error=%lu\n",__LINE__,GetLastError()); return 1; } } while(0)
static session owner;
static BOOL bound=TRUE,readable=TRUE,snapshot_ok=TRUE;
static DWORD palette_error=ERROR_SUCCESS,send_error,query_error;
static int query_result=1;
static LONG query_value;
static uint32_t columns=80,rows=50,storage_bytes=80*50*4;
static BYTE storage[80*50*4],sent[sizeof(console_text_style)+80*50*2];
static mvdm_softpc_text_video video;
static console_video_description description;
static PALETTEENTRY colours[16];
static unsigned publications;
session *session_thread_current(void) { return bound ? &owner : NULL; }
int session_presentation_text_describe(const session *s,uint32_t *w,uint32_t *h,uint32_t *n)
{ (void)s;*w=columns;*h=rows;*n=storage_bytes;return 1; }
int session_presentation_text_snapshot(const session *s,uint8_t *p,uint32_t capacity,uint32_t *w,uint32_t *h,uint32_t *n)
{ (void)s;(void)w;(void)h;(void)n;if(!snapshot_ok || capacity<storage_bytes)return 0;memcpy(p,storage,storage_bytes);return 1; }
int mvdm_softpc_text_video_copy(mvdm_softpc_text_video *p)
{ if(!readable)return 0;*p=video;return 1; }
BOOL ntvdm_console_text_palette(PALETTEENTRY p[16])
{ if(palette_error){SetLastError(palette_error);return FALSE;}memcpy(p,colours,sizeof(colours));return TRUE; }
int ntvdm_console_window_query(DWORD query,LONG values[4])
{ if(query!=CONSOLE_WINDOW_TEXT_FRAME_REQUIRED)return 0;values[0]=query_value;SetLastError(query_error);return query_result; }
BOOL ntvdm_console_publish_video(const console_video_description *d,const void *p,size_t capacity)
{
    if(send_error){SetLastError(send_error);return FALSE;}
    if(!d || d->bytes>sizeof(sent) || capacity!=d->bytes){SetLastError(ERROR_INVALID_DATA);return FALSE;}
    description=*d;memcpy(sent,p,d->bytes);++publications;return TRUE;
}
int main(void)
{
    BOOL requested;
    unsigned i;
    console_text_style *style=(console_text_style *)sent;
    owner.console_client=(void *)1;
    video.columns=80;video.rows=43;video.font_height=14;video.attribute_font_select=1;
    video.cursor_column=79;video.cursor_row=42;video.cursor_visible=1;
    video.cursor_start=12;video.cursor_height=2;video.cursor_start1=0;video.cursor_height1=1;
    video.fonts[1][65][13]=0xa5;colours[9].peRed=0x12;colours[9].peGreen=0x34;colours[9].peBlue=0x56;
    for(i=0;i<80*50;++i){storage[4*i]=(BYTE)i;storage[4*i+1]=9;storage[4*i+2]=0xee;storage[4*i+3]=0xff;}
    CHECK(NtvdmConsoleUpdateText(NULL) && publications==1);
    CHECK(description.kind==CONSOLE_VIDEO_TEXT_FRAME && description.width==80 && description.height==43 &&
        description.stride==160 && description.bytes==sizeof(*style)+80*43*2 && description.palette[9]==0x123456);
    CHECK(style->font_height==14 && style->attribute_font_select==1 && style->fonts[1][65][13]==0xa5);
    CHECK(style->cursor_column==79 && style->cursor_row==42 && style->cursor_start==12 &&
        style->cursor_height==2 && style->cursor_start1==0 && style->cursor_height1==1 && style->cursor_visible);
    for(i=0;i<80*43;++i)CHECK(sent[sizeof(*style)+2*i]==(BYTE)i && sent[sizeof(*style)+2*i+1]==9);
    /* Visible row width differs from backing stride, not a linear crop. */
    video.columns=40;video.rows=25;video.font_height=20;video.fonts[0][66][19]=0x5a;
    CHECK(NtvdmConsoleUpdateText(NULL) && publications==2);
    CHECK(style->fonts[0][66][19]==0x5a && description.width==40);
    for(i=0;i<40*25;++i)CHECK(sent[sizeof(*style)+2*i]==(BYTE)((i/40)*80+i%40));
    readable=FALSE;CHECK(NtvdmConsoleUpdateText(NULL) && publications==2);readable=TRUE;
    columns=0;CHECK(!NtvdmConsoleUpdateText(NULL) && GetLastError()==ERROR_INVALID_DATA);columns=80;
    ++storage_bytes;CHECK(!NtvdmConsoleUpdateText(NULL) && GetLastError()==ERROR_INVALID_DATA);--storage_bytes;
    snapshot_ok=FALSE;CHECK(!NtvdmConsoleUpdateText(NULL) && GetLastError()==ERROR_INVALID_DATA);snapshot_ok=TRUE;
    /* A late timer paint may lose DOS ownership after the original native
     * handoff. Drop only that paint; reclaim must publish a complete frame. */
    send_error=ERROR_NOT_READY;
    CHECK(NtvdmConsoleUpdateText(NULL) && publications==2);
    send_error=0;CHECK(NtvdmConsoleUpdateText(NULL) && publications==3);
    send_error=ERROR_INVALID_DATA;CHECK(!NtvdmConsoleUpdateText(NULL) && GetLastError()==ERROR_INVALID_DATA);
    send_error=ERROR_BROKEN_PIPE;CHECK(!NtvdmConsoleUpdateText(NULL) && GetLastError()==ERROR_BROKEN_PIPE);send_error=0;
    palette_error=ERROR_NO_DATA;
    CHECK(!NtvdmConsoleUpdateText(NULL) && GetLastError()==ERROR_INVALID_HANDLE);
    {
        struct { WORD version,count;PALETTEENTRY entries[16]; } palette={0x300,16,{0}};
        HPALETTE handle;
        palette.entries[9].peBlue=0x78;handle=CreatePalette((LOGPALETTE *)&palette);CHECK(handle);
        CHECK(NtvdmConsoleUpdateText(handle) && description.palette[9]==0x78);
        DeleteObject(handle);
    }
    palette_error=ERROR_ACCESS_DENIED;
    CHECK(!NtvdmConsoleUpdateText(NULL) && GetLastError()==ERROR_ACCESS_DENIED);
    bound=FALSE;CHECK(NtvdmConsoleUpdateText(NULL));bound=TRUE;
    owner.console_client=NULL;CHECK(NtvdmConsoleUpdateText(NULL));owner.console_client=(void *)1;
    query_result=-1;CHECK(NtvdmConsoleTextRequested(&requested) && !requested);
    query_result=1;query_value=1;CHECK(NtvdmConsoleTextRequested(&requested) && requested);
    query_value=0;CHECK(NtvdmConsoleTextRequested(&requested) && !requested);
    query_result=0;query_error=ERROR_NOT_READY;CHECK(NtvdmConsoleTextRequested(&requested) && !requested);
    query_error=ERROR_BROKEN_PIPE;CHECK(!NtvdmConsoleTextRequested(&requested) && GetLastError()==ERROR_BROKEN_PIPE);
    CHECK(!NtvdmConsoleTextRequested(NULL) && GetLastError()==ERROR_INVALID_PARAMETER);
    palette_error=0;snapshot_ok=FALSE;
    CHECK(NtvdmConsoleUpdateTextConfiguration(NULL));
    CHECK(description.kind==CONSOLE_VIDEO_TEXT_CONFIGURATION &&
        !description.width && !description.height && !description.stride &&
        description.bytes==sizeof(*style) && style->fonts[0][66][19]==0x5a &&
        !style->cursor_visible && description.palette[9]==0x123456);
    send_error=ERROR_NOT_READY;CHECK(NtvdmConsoleUpdateTextConfiguration(NULL));
    send_error=ERROR_BROKEN_PIPE;CHECK(!NtvdmConsoleUpdateTextConfiguration(NULL));send_error=0;
    palette_error=ERROR_NO_DATA;CHECK(NtvdmConsoleUpdateTextConfiguration(NULL));
    palette_error=ERROR_ACCESS_DENIED;CHECK(!NtvdmConsoleUpdateTextConfiguration(NULL));
    puts("PASS production text packer: original backing stride, visible extent, dual/tall fonts, split cursor, resolved/startup palette, refusal, transport errors and frontend demand");
    return 0;
}
