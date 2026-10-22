#define WIN32_LEAN_AND_MEAN
#include "window_frame.h"

static DWORD decode_text_frame(const frontend_video *video,kvm_window_frame_storage *storage)
{
    const console_video_description *d=&video->description;
    const console_text_style *style;
    const BYTE *cells;
    kvm_window_text_frame *text;
    unsigned i,cell_bytes;
    lib_status status;
    /* Existing wire limits, not public-library allocation limits. */
    if(!d->width || d->width>160 || !d->height || d->height>96 || d->depth ||
        (d->stride!=d->width*2 && d->stride!=d->width*3) ||
        d->bytes!=sizeof(*style)+d->stride*d->height)return ERROR_INVALID_DATA;
    style=(const console_text_style *)video->pixels;
    cells=video->pixels+sizeof(*style);
    cell_bytes=d->stride/d->width;
    if(!style->font_height || style->font_height>32 ||
        style->attribute_font_select>1 || style->cursor_visible>1 ||
        style->cursor_height<0 || style->cursor_height>32 ||
        style->cursor_height1<0 || style->cursor_height1>32 ||
        style->cursor_start < -32 || style->cursor_start>31 ||
        style->cursor_start1 < -32 || style->cursor_start1>31)return ERROR_INVALID_DATA;
    if(cell_bytes==3)for(i=0;i<d->width*d->height;++i)
        if(cells[i*3+2]&~CONSOLE_TEXT_STYLE_MASK)return ERROR_INVALID_DATA;
    status=kvm_window_frame_prepare_text(storage,d->width,d->height,d->width);
    if(status!=LIB_STATUS_OK)return status==LIB_STATUS_NO_MEMORY ? ERROR_NOT_ENOUGH_MEMORY : ERROR_INVALID_DATA;
    text=storage->data.text;
    text->base.font_height=style->font_height;
    text->base.cursor_column=style->cursor_column;
    text->base.cursor_row=style->cursor_row;
    text->base.cursor_mode=style->cursor_visible ? KVM_CURSOR_HOST : KVM_CURSOR_DISABLED;
    text->base.cursor_phase=LIB_TRUE;
    text->base.cursor[0]=(kvm_text_cursor_span){(lib_u32)style->cursor_start,(lib_u32)style->cursor_height};
    text->base.cursor[1]=(kvm_text_cursor_span){(lib_u32)style->cursor_start1,(lib_u32)style->cursor_height1};
    text->base.underline_row=style->font_height-1;
    memcpy(text->base.text_palette,d->palette,sizeof(text->base.text_palette));
    memcpy(text->font,style->fonts[0],sizeof(text->font));
    memcpy(text->secondary_font,style->fonts[1],sizeof(text->secondary_font));
    for(i=0;i<d->width*d->height;++i) {
        BYTE attribute=cells[i*cell_bytes+1];
        text->base.cells[i]=(kvm_text_cell){cells[i*cell_bytes],
            style->attribute_font_select && (attribute&8) ? 1 : 0,
            attribute&15,attribute>>4,
            cell_bytes==3 && (cells[i*3+2]&CONSOLE_TEXT_UNDERLINE) ? 1 : 0};
    }
    return ERROR_SUCCESS;
}

DWORD frontend_window_decode_frame(const frontend_video *video,kvm_window_frame_storage *storage)
{
    const console_video_description *d;
    uint64_t stride;
    unsigned x,y;
    DWORD error;
    lib_status status;
    if(!video || !storage)return ERROR_INVALID_PARAMETER;
    if(!video->pixels || !video->published_serial)return ERROR_NO_DATA;
    d=&video->description;
    if(d->kind==CONSOLE_VIDEO_TEXT_FRAME) {
        error=decode_text_frame(video,storage);
        if(error)return error;
    } else {
        if(d->kind!=CONSOLE_VIDEO_DIB || !d->width || !d->height ||
            (d->depth!=1 && d->depth!=8))return ERROR_INVALID_DATA;
        stride=(((uint64_t)d->width*d->depth+31)/32)*4;
        if(stride!=d->stride || stride*d->height!=d->bytes)return ERROR_INVALID_DATA;
        status=kvm_window_frame_prepare_graphics(storage,d->width,d->height,d->width);
        if(status!=LIB_STATUS_OK)return status==LIB_STATUS_NO_MEMORY ? ERROR_NOT_ENOUGH_MEMORY : ERROR_INVALID_DATA;
        /* Wire: top-down, DWORD-padded palette indices. Library: RGB pixels. */
        for(y=0;y<d->height;++y) {
            const BYTE *source=video->pixels+(SIZE_T)y*d->stride;
            lib_u32 *target=storage->data.graphics->pixels+(SIZE_T)y*d->width;
            for(x=0;x<d->width;++x)
                target[x]=d->palette[d->depth==8 ? source[x] : (source[x/8]>>(7-x%8))&1];
        }
    }
    storage->frame.sequence=video->published_serial;
    return kvm_window_frame_validate(&storage->frame)==LIB_STATUS_OK ? ERROR_SUCCESS : ERROR_INVALID_DATA;
}
