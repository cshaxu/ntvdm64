#include "text_frame.h"
#include <string.h>

/* Recovered from ntkvm-exe/window_frame.c native_glyph. Same bounded PC
 * mapping, now at its backend owner. Unsupported Unicode is one '?' cell;
 * the second cell of a wide/surrogate pair remains blank. */
static BYTE pc_glyph(WCHAR character)
{
    static const WCHAR pictures[31]={
        0x263a,0x263b,0x2665,0x2666,0x2663,0x2660,0x2022,0x25d8,
        0x25cb,0x25d9,0x2642,0x2640,0x266a,0x266b,0x263c,0x25ba,
        0x25c4,0x2195,0x203c,0x00b6,0x00a7,0x25ac,0x21a8,0x2191,
        0x2193,0x2192,0x2190,0x221f,0x2194,0x25b2,0x25bc};
    unsigned i;char byte='?';BOOL replaced=FALSE;
    if(!character || character==L' ')return ' ';
    for(i=0;i<31;++i)if(character==pictures[i])return (BYTE)(i+1);
    if(character==0x2302)return 127;
    if(character<32 || character==127)return '?';
    if(WideCharToMultiByte(437,WC_NO_BEST_FIT_CHARS,&character,1,&byte,1,"?",&replaced)!=1 || replaced)
        return '?';
    return (BYTE)byte;
}

DWORD ntcon_text_frame_pack(const CONSOLE_SCREEN_BUFFER_INFOEX *screen,
    const CONSOLE_CURSOR_INFO *cursor,const CHAR_INFO *cells,SIZE_T count,
    const console_text_style *font,console_video_description *description,BYTE **payload)
{
    console_video_description result={0};console_text_style *style;
    BYTE *bytes;unsigned columns,rows,x,y,i,cell_bytes=2;
    if(!payload || !description)return ERROR_INVALID_PARAMETER;
    *payload=NULL;ZeroMemory(description,sizeof(*description));
    if(!screen || !cursor || !cells || !font)return ERROR_INVALID_PARAMETER;
    if(screen->dwSize.X<=0 || screen->dwSize.Y<=0 ||
        (SIZE_T)screen->dwSize.X*screen->dwSize.Y!=count ||
        screen->srWindow.Left<0 || screen->srWindow.Top<0 ||
        screen->srWindow.Right<screen->srWindow.Left ||
        screen->srWindow.Bottom<screen->srWindow.Top ||
        screen->srWindow.Right>=screen->dwSize.X || screen->srWindow.Bottom>=screen->dwSize.Y ||
        !font->font_height || font->font_height>32 || font->attribute_font_select>1 ||
        !cursor->dwSize || cursor->dwSize>100)return ERROR_INVALID_DATA;
    columns=screen->srWindow.Right-screen->srWindow.Left+1;
    rows=screen->srWindow.Bottom-screen->srWindow.Top+1;
    /* Match the existing receiver, rather than introduce a native-only limit. */
    if(columns>160 || rows>96 || rows>768/font->font_height)return ERROR_NOT_SUPPORTED;
    for(y=0;y<rows;++y)for(x=0;x<columns;++x)
        if(cells[(SIZE_T)(y+screen->srWindow.Top)*screen->dwSize.X+screen->srWindow.Left+x].Attributes&
            COMMON_LVB_UNDERSCORE)
            cell_bytes=3;
    result.kind=CONSOLE_VIDEO_TEXT_FRAME;result.width=columns;result.height=rows;
    result.stride=columns*cell_bytes;result.bytes=sizeof(*style)+result.stride*rows;
    bytes=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,result.bytes);
    if(!bytes)return ERROR_NOT_ENOUGH_MEMORY;
    style=(console_text_style *)bytes;
    style->font_height=font->font_height;style->attribute_font_select=font->attribute_font_select;
    memcpy(style->fonts,font->fonts,sizeof(style->fonts));
    style->cursor_column=screen->dwCursorPosition.X-screen->srWindow.Left;
    style->cursor_row=screen->dwCursorPosition.Y-screen->srWindow.Top;
    style->cursor_visible=cursor->bVisible!=FALSE;
    style->cursor_height=(cursor->dwSize*font->font_height+99)/100;
    style->cursor_start=font->font_height-style->cursor_height;
    for(i=0;i<16;++i) {
        COLORREF color=screen->ColorTable[i];
        result.palette[i]=((DWORD)GetRValue(color)<<16)|((DWORD)GetGValue(color)<<8)|GetBValue(color);
    }
    for(y=0;y<rows;++y)for(x=0;x<columns;++x) {
        const CHAR_INFO *source=cells+(SIZE_T)(y+screen->srWindow.Top)*screen->dwSize.X+screen->srWindow.Left+x;
        BYTE attribute=(BYTE)source->Attributes;
        BYTE *destination=bytes+sizeof(*style)+(y*columns+x)*cell_bytes;
        if(source->Attributes&(COMMON_LVB_GRID_HORIZONTAL|COMMON_LVB_GRID_LVERTICAL|COMMON_LVB_GRID_RVERTICAL)) {
            HeapFree(GetProcessHeap(),0,bytes);return ERROR_NOT_SUPPORTED;
        }
        if(cell_bytes==3) {
            destination[2]=source->Attributes&COMMON_LVB_UNDERSCORE ? CONSOLE_TEXT_UNDERLINE : 0;
        }
        if(source->Attributes&COMMON_LVB_REVERSE_VIDEO)attribute=(BYTE)((attribute<<4)|(attribute>>4));
        destination[0]=pc_glyph(source->Char.UnicodeChar);destination[1]=attribute;
        if(source->Attributes&COMMON_LVB_TRAILING_BYTE)destination[0]=' ';
        if((x || screen->srWindow.Left) && source->Char.UnicodeChar>=0xdc00 && source->Char.UnicodeChar<=0xdfff &&
            source[-1].Char.UnicodeChar>=0xd800 && source[-1].Char.UnicodeChar<=0xdbff)destination[0]=' ';
    }
    *description=result;*payload=bytes;return 0;
}
