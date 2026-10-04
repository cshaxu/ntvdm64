#define WIN32_LEAN_AND_MEAN
#include "window_frame.h"
#include "text_frame.h"


static DWORD decode_text_frame(const frontend_video *video,kvm_window_frame *frame)
{
    const console_video_description *d=&video->description;
    const console_text_style *style=(const console_text_style *)video->pixels;
    const BYTE *cells=video->pixels+sizeof(*style);
    frontend_text_snapshot *snapshot=NULL;
    unsigned i,bank,glyph,line,cell_bytes;DWORD error=ERROR_NOT_ENOUGH_MEMORY;
    if(!d->width || d->width>160 || !d->height || d->height>96 || d->depth ||
        (d->stride!=d->width*2 && d->stride!=d->width*3) || d->bytes!=sizeof(*style)+d->stride*d->height)
        return ERROR_INVALID_DATA;
    cell_bytes=d->stride/d->width;
    if(!style->font_height || style->font_height>32 ||
        style->attribute_font_select>1 || style->cursor_visible>1 ||
        style->cursor_height<0 || style->cursor_height>32 || style->cursor_height1<0 || style->cursor_height1>32 ||
        style->cursor_start < -32 || style->cursor_start>31 ||
        style->cursor_start1 < -32 || style->cursor_start1>31)return ERROR_INVALID_DATA;
    snapshot=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*snapshot));
    if(!snapshot)goto done;
    snapshot->cell_count=d->width*d->height;
    if(cell_bytes==3) {
        for(i=0;i<snapshot->cell_count;++i)if(cells[i*3+2]&~CONSOLE_TEXT_STYLE_MASK) {
            error=ERROR_INVALID_DATA;goto done;
        }
        snapshot->extension.cell_styles=cells+2;
        snapshot->extension.cell_style_stride=3;
    }
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
        BYTE *font=bank ? snapshot->fonts.secondary_font : snapshot->fonts.font;
        font[glyph*KVM_WINDOW_FONT_HEIGHT+line]=style->fonts[bank][glyph][line];
    }
    for(i=0;i<snapshot->cell_count;++i) {
        BYTE attribute=cells[i*cell_bytes+1];
        snapshot->cells[i].glyph_index=cells[i*cell_bytes];
        snapshot->cells[i].glyph_bank=style->attribute_font_select && (attribute&8) ? 1 : 0;
        snapshot->cells[i].foreground=attribute&15;snapshot->cells[i].background=attribute>>4;
    }
    error=frontend_text_frame_prepare(&snapshot->fonts,snapshot->cells,
        snapshot->cell_count,&snapshot->extension,TRUE,frame) ? ERROR_SUCCESS : GetLastError();
done:
    if(snapshot)HeapFree(GetProcessHeap(),0,snapshot);
    return error;
}
DWORD frontend_window_decode_frame(const frontend_video *video, kvm_window_frame *frame)
{
    const console_video_description *description;
    uint64_t stride;
    unsigned x, y;
    if (!frame) return ERROR_INVALID_PARAMETER;
    frame->valid = LIB_FALSE;
    if (!video) return ERROR_INVALID_PARAMETER;
    if (!video->pixels || !video->published_serial) return ERROR_NO_DATA;
    description = &video->description;
    if(description->kind==CONSOLE_VIDEO_TEXT_FRAME)return decode_text_frame(video,frame);
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
