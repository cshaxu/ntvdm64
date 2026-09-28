#define WIN32_LEAN_AND_MEAN
#include "window_frame.h"
#include "text_frame.h"
#include "native_pc_font.h"

/* Inverse of the traditional PC bitmap map used by SoftPC presentation.
 * CP437 is exact (no best fit); unsupported Unicode stays in terminal state
 * and becomes '?' only at this bounded bitmap presentation boundary. */
static BYTE native_glyph(WCHAR character)
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

/* Preserve the already accepted system-arrow presentation. GDI draws only
 * this pointer, never a second font renderer. */
static DWORD native_arrow(kvm_window_frame *frame,const POINT *pointer)
{
    HDC dc=NULL;HBITMAP bitmap=NULL;HGDIOBJ previous=NULL;
    BITMAPINFO bmi={0};DWORD *pixels=NULL,error=ERROR_GEN_FAILURE;
    unsigned n,i,colours=32,width=frame->image.width,height=frame->image.height;
    HCURSOR arrow;ICONINFO icon={0};
    if(!pointer)return 0;
    dc=CreateCompatibleDC(NULL);if(!dc)goto done;
    bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth=(LONG)width;bmi.bmiHeader.biHeight=-(LONG)height;
    bmi.bmiHeader.biPlanes=1;bmi.bmiHeader.biBitCount=32;bmi.bmiHeader.biCompression=BI_RGB;
    bitmap=CreateDIBSection(dc,&bmi,DIB_RGB_COLORS,(void **)&pixels,NULL,0);
    if(!bitmap)goto done;
    previous=SelectObject(dc,bitmap);if(!previous || previous==HGDI_ERROR)goto done;
    for(n=0;n<width*height;++n)pixels[n]=frame->image.palette[frame->image.pixels[n]];
    arrow=(HCURSOR)LoadImageW(NULL,MAKEINTRESOURCEW(32512),IMAGE_CURSOR,0,0,
        LR_DEFAULTSIZE|LR_SHARED|LR_MONOCHROME);
    if(!arrow || !GetIconInfo(arrow,&icon))goto done;
    if(!DrawIconEx(dc,pointer->x-(int)icon.xHotspot,pointer->y-(int)icon.yHotspot,
        arrow,0,0,0,NULL,DI_NORMAL) || !GdiFlush())goto done;
    for(n=0;n<width*height;++n) {
        DWORD rgb=pixels[n]&0xffffff;
        for(i=0;i<colours && rgb!=frame->image.palette[i];++i) {}
        if(i==colours) {
            if(colours==KVM_WINDOW_GRAPHICS_PALETTE_ENTRIES) {error=ERROR_NOT_SUPPORTED;goto done;}
            frame->image.palette[colours++]=rgb;
        }
        frame->image.pixels[n]=(lib_u8)i;
    }
    error=0;
done:
    if(icon.hbmMask)DeleteObject(icon.hbmMask);
    if(icon.hbmColor)DeleteObject(icon.hbmColor);
    if(previous && previous!=HGDI_ERROR)SelectObject(dc,previous);
    if(bitmap)DeleteObject(bitmap);
    if(dc)DeleteDC(dc);
    return error;
}

DWORD frontend_window_native_frame_pointer(const run16_native_frame_info *text,
    const CHAR_INFO *buffer,SIZE_T count,const POINT *pointer,kvm_window_frame *frame)
{
    frontend_text_snapshot *snapshot=NULL;frontend_text_raster *scratch=NULL;
    unsigned columns,rows,x,y,i;DWORD error=ERROR_NOT_ENOUGH_MEMORY;
    if(!frame)return ERROR_INVALID_PARAMETER;
    frame->valid=0;
    if(!text || !buffer)return ERROR_INVALID_PARAMETER;
    if(text->screen.dwSize.X<=0 || text->screen.dwSize.Y<=0 ||
        (SIZE_T)text->screen.dwSize.X*text->screen.dwSize.Y!=count ||
        text->screen.srWindow.Left<0 || text->screen.srWindow.Top<0 ||
        text->screen.srWindow.Right<text->screen.srWindow.Left ||
        text->screen.srWindow.Bottom<text->screen.srWindow.Top ||
        text->screen.srWindow.Right>=text->screen.dwSize.X ||
        text->screen.srWindow.Bottom>=text->screen.dwSize.Y)return ERROR_INVALID_DATA;
    columns=text->screen.srWindow.Right-text->screen.srWindow.Left+1;
    rows=text->screen.srWindow.Bottom-text->screen.srWindow.Top+1;
    if(columns>KVM_WINDOW_GRAPHICS_MAX_WIDTH/8 || rows>KVM_WINDOW_GRAPHICS_MAX_HEIGHT/14 ||
        !text->cursor.dwSize || text->cursor.dwSize>100)return ERROR_NOT_SUPPORTED;
    snapshot=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*snapshot));
    scratch=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*scratch));
    if(!snapshot || !scratch)goto done;
    snapshot->cell_count=columns*rows;
    snapshot->fonts.base.text_columns=(lib_u16)columns;
    snapshot->fonts.base.text_rows=(lib_u16)rows;
    snapshot->fonts.base.font_height=14;
    for(i=0;i<16;++i) {
        COLORREF colour=text->screen.ColorTable[i];
        snapshot->fonts.base.text_palette[i]=((DWORD)GetRValue(colour)<<16)|
            ((DWORD)GetGValue(colour)<<8)|GetBValue(colour);
    }
    for(i=0;i<256;++i) {
        memcpy(snapshot->fonts.font+i*16,frontend_native_font[i],14);
        memcpy(snapshot->fonts.secondary_font+i*16,frontend_native_font[i],14);
        snapshot->fonts.secondary_font[i*16+13]=0xff;
    }
    for(y=0;y<rows;++y)for(x=0;x<columns;++x) {
        const CHAR_INFO *source=buffer+(SIZE_T)(y+text->screen.srWindow.Top)*text->screen.dwSize.X+
            text->screen.srWindow.Left+x;
        kvm_text_cell *cell=snapshot->cells+y*columns+x;
        cell->glyph_index=' ';
        cell->foreground=source->Attributes&15;cell->background=(source->Attributes>>4)&15;
        if(source->Attributes&COMMON_LVB_REVERSE_VIDEO) {
            lib_u8 swap=cell->foreground;cell->foreground=cell->background;cell->background=swap;
        }
        cell->glyph_bank=!!(source->Attributes&COMMON_LVB_UNDERSCORE);
    }
    for(y=0;y<rows;++y)for(x=0;x<columns;++x) {
        const CHAR_INFO *source=buffer+(SIZE_T)(y+text->screen.srWindow.Top)*text->screen.dwSize.X+
            text->screen.srWindow.Left+x;
        WCHAR ch=source->Char.UnicodeChar;
        snapshot->cells[y*columns+x].glyph_index=native_glyph(ch);
        if(x+1<columns && (((source->Attributes&COMMON_LVB_LEADING_BYTE) &&
            (source[1].Attributes&COMMON_LVB_TRAILING_BYTE)) ||
            (ch>=0xd800 && ch<=0xdbff && source[1].Char.UnicodeChar>=0xdc00 &&
                source[1].Char.UnicodeChar<=0xdfff)))++x;
    }
    snapshot->fonts.base.cursor_column=text->screen.dwCursorPosition.X-text->screen.srWindow.Left;
    snapshot->fonts.base.cursor_row=text->screen.dwCursorPosition.Y-text->screen.srWindow.Top;
    snapshot->fonts.base.cursor_visible=text->cursor.bVisible;
    snapshot->fonts.base.cursor_top=(lib_u8)(14-(text->cursor.dwSize*14+99)/100);
    snapshot->fonts.base.cursor_bottom=13;
    if(!frontend_text_frame_rasterize(scratch,&snapshot->fonts,snapshot->cells,
        snapshot->cell_count,NULL,TRUE,frame)) {error=GetLastError();goto done;}
    error=native_arrow(frame,pointer);
done:
    if(error)frame->valid=0;
    if(scratch)HeapFree(GetProcessHeap(),0,scratch);
    if(snapshot)HeapFree(GetProcessHeap(),0,snapshot);
    return error;
}
DWORD frontend_window_native_frame(const run16_native_frame_info *text,
    const CHAR_INFO *buffer,SIZE_T count,kvm_window_frame *frame)
{
    return frontend_window_native_frame_pointer(text,buffer,count,NULL,frame);
}

static DWORD dos_text_frame(const run16_console_video *video,kvm_window_frame *frame)
{
    const console_video_description *d=&video->description;
    const console_text_style *style=(const console_text_style *)video->pixels;
    const BYTE *cells=video->pixels+sizeof(*style);
    frontend_text_snapshot *snapshot=NULL;frontend_text_raster *scratch=NULL;
    unsigned i,bank,glyph,line;DWORD error=ERROR_NOT_ENOUGH_MEMORY;
    if(!d->width || d->width>160 || !d->height || d->height>96 || d->depth ||
        d->stride!=d->width*2 || d->bytes!=sizeof(*style)+d->stride*d->height)
        return ERROR_INVALID_DATA;
    if(!style->font_height || style->font_height>32 || d->height>768/style->font_height ||
        style->attribute_font_select>1 || style->cursor_visible>1 ||
        style->cursor_height<0 || style->cursor_height>32 || style->cursor_height1<0 || style->cursor_height1>32 ||
        style->cursor_start < -32 || style->cursor_start>31 ||
        style->cursor_start1 < -32 || style->cursor_start1>31)return ERROR_INVALID_DATA;
    snapshot=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*snapshot));
    scratch=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*scratch));
    if(!snapshot || !scratch)goto done;
    snapshot->cell_count=d->width*d->height;
    snapshot->fonts.base.text_columns=(lib_u16)d->width;
    snapshot->fonts.base.text_rows=(lib_u16)d->height;
    snapshot->fonts.base.font_height=style->font_height;
    snapshot->fonts.base.cursor_column=style->cursor_column;
    snapshot->fonts.base.cursor_row=style->cursor_row;
    snapshot->fonts.base.cursor_visible=style->cursor_visible && style->cursor_height>0 &&
        style->cursor_start>=0 && style->cursor_start<(int)style->font_height;
    snapshot->fonts.base.cursor_top=(lib_u8)style->cursor_start;
    snapshot->fonts.base.cursor_bottom=(lib_u8)(style->cursor_start+style->cursor_height-1);
    snapshot->extension.cursor_visible=style->cursor_visible && style->cursor_height1>0 &&
        style->cursor_start1>=0 && style->cursor_start1<(int)style->font_height;
    snapshot->extension.cursor_top=(lib_u8)style->cursor_start1;
    snapshot->extension.cursor_bottom=(lib_u8)(style->cursor_start1+style->cursor_height1-1);
    memcpy(snapshot->fonts.base.text_palette,d->palette,sizeof(snapshot->fonts.base.text_palette));
    for(bank=0;bank<2;++bank)for(glyph=0;glyph<256;++glyph)for(line=0;line<style->font_height;++line) {
        BYTE *font=line>=16 ? snapshot->extension.upper_font[bank] :
            bank ? snapshot->fonts.secondary_font : snapshot->fonts.font;
        font[glyph*16+(line%16)]=style->fonts[bank][glyph][line];
    }
    for(i=0;i<snapshot->cell_count;++i) {
        BYTE attribute=cells[i*2+1];
        snapshot->cells[i].glyph_index=cells[i*2];
        snapshot->cells[i].glyph_bank=style->attribute_font_select && (attribute&8) ? 1 : 0;
        snapshot->cells[i].foreground=attribute&15;snapshot->cells[i].background=attribute>>4;
    }
    error=frontend_text_frame_prepare(scratch,&snapshot->fonts,snapshot->cells,
        snapshot->cell_count,&snapshot->extension,TRUE,frame) ? ERROR_SUCCESS : GetLastError();
done:
    if(scratch)HeapFree(GetProcessHeap(),0,scratch);
    if(snapshot)HeapFree(GetProcessHeap(),0,snapshot);
    return error;
}
DWORD frontend_window_dos_frame(const run16_console_video *video, kvm_window_frame *frame)
{
    const console_video_description *description;
    uint64_t stride;
    unsigned x, y;
    if (!frame) return ERROR_INVALID_PARAMETER;
    frame->valid = LIB_FALSE;
    if (!video) return ERROR_INVALID_PARAMETER;
    if (!video->pixels || !video->published_serial) return ERROR_NO_DATA;
    description = &video->description;
    if(description->kind==CONSOLE_VIDEO_TEXT_FRAME)return dos_text_frame(video,frame);
    if(description->kind!=CONSOLE_VIDEO_DIB)return ERROR_INVALID_DATA;
    if (!description->width || !description->height ||
        (description->depth != 1 && description->depth != 8)) return ERROR_INVALID_DATA;
    stride = (((uint64_t)description->width * description->depth + 31) / 32) * 4;
    if (stride != description->stride || stride * description->height != description->bytes)
        return ERROR_INVALID_DATA;
    if (description->width > KVM_WINDOW_GRAPHICS_MAX_WIDTH ||
        description->height > KVM_WINDOW_GRAPHICS_MAX_HEIGHT) return ERROR_NOT_SUPPORTED;
    /* The existing wire ABI is top-down, DWORD-padded DIB data. Preserve row
     * order and palette; unpack monochrome MSB-first without copying padding. */
    for (y = 0; y < description->height; ++y) {
        const BYTE *source = video->pixels + (SIZE_T)y * description->stride;
        lib_u8 *target = frame->image.pixels + (SIZE_T)y * description->width;
        if (description->depth == 8) memcpy(target, source, description->width);
        else for (x = 0; x < description->width; ++x)
            target[x] = (source[x / 8] >> (7 - x % 8)) & 1;
    }
    memcpy(frame->image.palette, description->palette, sizeof(frame->image.palette));
    frame->image.width = frame->image.stride = description->width;
    frame->image.height = description->height;
    frame->graphics = LIB_TRUE;
    frame->valid = LIB_TRUE;
    return ERROR_SUCCESS;
}
