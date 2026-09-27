#define WIN32_LEAN_AND_MEAN
#include "window_frame.h"
#include "text_frame.h"

/* Source recovery: reference codex/t423-original-reference-20260925,
 * native_console_frame.c. Keep GDI Unicode/palette/cursor conversion;
 * consume the current full copied buffer instead of the retired viewport ABI.
 * This is frontend-local presentation, never guest graphics mode. */
/* Native text is Unicode, not a DOS font-bank index. Windows owns the cells
 * and attributes; this boundary only rasterizes the visible viewport. */
DWORD frontend_window_native_frame_pointer(const run16_native_frame_info *text,
    const CHAR_INFO *buffer, SIZE_T count, const POINT *pointer, kvm_window_frame *frame)
{
    const CHAR_INFO *cells;
    HDC dc = NULL;
    HFONT font = NULL;
    HBITMAP bitmap = NULL;
    HGDIOBJ old_font = NULL, old_bitmap = NULL;
    BITMAPINFO bmi = {0};
    DWORD *pixels = NULL, error = ERROR_GEN_FAILURE;
    unsigned columns, rows, width, height, x, y, n, i;
    const unsigned cw = FRONTEND_NATIVE_CELL_WIDTH, ch = FRONTEND_NATIVE_CELL_HEIGHT;
    if (!frame) return ERROR_INVALID_PARAMETER;
    frame->valid = 0;
    if (!text || !buffer) return ERROR_INVALID_PARAMETER;
    if (text->screen.dwSize.X <= 0 || text->screen.dwSize.Y <= 0 ||
        (SIZE_T)text->screen.dwSize.X * text->screen.dwSize.Y != count ||
        text->screen.srWindow.Left < 0 || text->screen.srWindow.Top < 0 ||
        text->screen.srWindow.Right < text->screen.srWindow.Left ||
        text->screen.srWindow.Bottom < text->screen.srWindow.Top ||
        text->screen.srWindow.Right >= text->screen.dwSize.X ||
        text->screen.srWindow.Bottom >= text->screen.dwSize.Y)
        return ERROR_INVALID_DATA;
    columns = text->screen.srWindow.Right - text->screen.srWindow.Left + 1;
    rows = text->screen.srWindow.Bottom - text->screen.srWindow.Top + 1;
    if (!columns || !rows || !text->cursor.dwSize || text->cursor.dwSize > 100) {
        error = ERROR_NOT_SUPPORTED; goto done;
    }
    width = columns * cw; height = rows * ch;
    /* Rasterize at the nominal cell size. The Window library alone fits its
     * client area to the monitor; its transport capacity is not a font policy.
     * Console scrollback is independent of this Window-only conversion. */
    if (width > KVM_WINDOW_GRAPHICS_MAX_WIDTH || height > KVM_WINDOW_GRAPHICS_MAX_HEIGHT) {
        error = ERROR_NOT_SUPPORTED; goto done;
    }
    cells = buffer + (SIZE_T)text->screen.srWindow.Top * text->screen.dwSize.X +
        text->screen.srWindow.Left;
    dc = CreateCompatibleDC(NULL);
    if (!dc) goto done;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = (LONG)width;
    bmi.bmiHeader.biHeight = -(LONG)height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bitmap = CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, (void **)&pixels, NULL, 0);
    font = CreateFontW(-(int)ch, cw, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        NONANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN, L"Consolas");
    if (!bitmap || !font) goto done;
    old_font = SelectObject(dc, font); old_bitmap = SelectObject(dc, bitmap);
    if (!old_font || old_font == HGDI_ERROR || !old_bitmap || old_bitmap == HGDI_ERROR) goto done;
    ZeroMemory(frame->image.palette, sizeof(frame->image.palette));
    for (i = 0; i < 16; ++i) {
        COLORREF color = text->screen.ColorTable[i];
        frame->image.palette[i] = ((DWORD)GetRValue(color) << 16) |
            ((DWORD)GetGValue(color) << 8) | GetBValue(color);
        frame->image.palette[16 + i] = frame->image.palette[i] ^ 0xffffff;
    }
    for (y = 0; y < rows; ++y) for (x = 0; x < columns; ++x) {
        const CHAR_INFO *cell = cells + (SIZE_T)y * text->screen.dwSize.X + x;
        unsigned fg = cell->Attributes & 15, bg = (cell->Attributes >> 4) & 15;
        unsigned span = 1, length = 1;
        WCHAR glyph[2] = {cell->Char.UnicodeChar, 0};
        RECT rect = {(LONG)(x * width / columns), (LONG)(y * height / rows),
            0, (LONG)((y + 1) * height / rows)};
        if (cell->Attributes & COMMON_LVB_REVERSE_VIDEO) { unsigned tmp = fg; fg = bg; bg = tmp; }
        if ((cell->Attributes & COMMON_LVB_LEADING_BYTE) && x + 1 < columns &&
            (cell[1].Attributes & COMMON_LVB_TRAILING_BYTE)) span = 2;
        else if (glyph[0] >= 0xd800 && glyph[0] <= 0xdbff && x + 1 < columns &&
            cell[1].Char.UnicodeChar >= 0xdc00 && cell[1].Char.UnicodeChar <= 0xdfff) {
            span = 2; length = 2; glyph[1] = cell[1].Char.UnicodeChar;
        }
        rect.right = (LONG)((x + span) * width / columns);
        SetTextColor(dc, text->screen.ColorTable[fg]); SetBkColor(dc, text->screen.ColorTable[bg]);
        if (rect.right > rect.left && rect.bottom > rect.top &&
            !ExtTextOutW(dc, rect.left, rect.top, ETO_CLIPPED | ETO_OPAQUE,
            &rect, glyph, length, NULL)) goto done;
        x += span - 1;
    }
    if (!GdiFlush()) goto done;
    /* Flush the complete raster once before direct DIB access, not per cell. */
    for (y = 0; y < rows; ++y) for (x = 0; x < columns; ++x) {
        WORD attributes = cells[(SIZE_T)y * text->screen.dwSize.X + x].Attributes;
        unsigned fg = attributes & COMMON_LVB_REVERSE_VIDEO ?
            (attributes >> 4) & 15 : attributes & 15;
        unsigned top = y * height / rows, bottom = (y + 1) * height / rows;
        if ((attributes & COMMON_LVB_UNDERSCORE) && bottom > top)
            for (n = x * width / columns; n < (x + 1) * width / columns; ++n)
                pixels[(bottom - 1) * width + n] = frame->image.palette[fg];
    }
    /* NONANTIALIASED glyphs contain only Console palette colors. Do not
     * silently quantize unexpected colors and mask a broken conversion. */
    for (n = 0; n < width * height; ++n) {
        DWORD rgb = pixels[n] & 0xffffff;
        for (i = 0; i < 16 && rgb != frame->image.palette[i]; ++i) {}
        if (i == 16) { error = ERROR_INVALID_DATA; goto done; }
        frame->image.pixels[n] = (lib_u8)i;
    }
    if (text->cursor.bVisible && text->screen.dwCursorPosition.X >= text->screen.srWindow.Left &&
        text->screen.dwCursorPosition.X <= text->screen.srWindow.Right && text->screen.dwCursorPosition.Y >= text->screen.srWindow.Top &&
        text->screen.dwCursorPosition.Y <= text->screen.srWindow.Bottom) {
        unsigned left, right, top, bottom, lines;
        x = text->screen.dwCursorPosition.X - text->screen.srWindow.Left;
        y = text->screen.dwCursorPosition.Y - text->screen.srWindow.Top;
        left = x * width / columns; right = (x + 1) * width / columns;
        top = y * height / rows; bottom = (y + 1) * height / rows;
        lines = (text->cursor.dwSize * (bottom - top) + 99) / 100;
        for (n = bottom - lines; n < bottom; ++n)
            for (i = left; i < right; ++i) frame->image.pixels[n * width + i] += 16;
    }
    if(pointer) {
        /* Presentation-only system arrow on the copied raster, never the
         * Console buffer or guest memory. Themes may still return colored
         * pixels; retain their exact colors in the existing indexed carrier. */
        HCURSOR arrow=(HCURSOR)LoadImageW(NULL,MAKEINTRESOURCEW(32512),IMAGE_CURSOR,
            0,0,LR_DEFAULTSIZE|LR_SHARED|LR_MONOCHROME);
        ICONINFO icon={0};BOOL drawn;unsigned colors=32;
        if(!arrow || !GetIconInfo(arrow,&icon)) { error=GetLastError();if(!error)error=ERROR_GEN_FAILURE;goto done; }
        for(n=0;n<width*height;++n)pixels[n]=frame->image.palette[frame->image.pixels[n]];
        drawn=DrawIconEx(dc,pointer->x-(int)icon.xHotspot,pointer->y-(int)icon.yHotspot,
            arrow,0,0,0,NULL,DI_NORMAL);
        if(icon.hbmMask)DeleteObject(icon.hbmMask);
        if(icon.hbmColor)DeleteObject(icon.hbmColor);
        if(!drawn || !GdiFlush()) { error=GetLastError();if(!error)error=ERROR_GEN_FAILURE;goto done; }
        for(n=0;n<width*height;++n) {
            DWORD rgb=pixels[n]&0xffffff;
            for(i=0;i<colors && rgb!=frame->image.palette[i];++i){}
            if(i==colors) {
                if(colors==KVM_WINDOW_GRAPHICS_PALETTE_ENTRIES) { error=ERROR_NOT_SUPPORTED;goto done; }
                frame->image.palette[colors++]=rgb;
            }
            frame->image.pixels[n]=(lib_u8)i;
        }
    }
    frame->graphics = 1;
    frame->image.width = frame->image.stride = width;
    frame->image.height = height;
    frame->valid = 1;
    error = ERROR_SUCCESS;
done:
    if (old_font && old_font != HGDI_ERROR) SelectObject(dc, old_font);
    if (old_bitmap && old_bitmap != HGDI_ERROR) SelectObject(dc, old_bitmap);
    if (font) DeleteObject(font);
    if (bitmap) DeleteObject(bitmap);
    if (dc) DeleteDC(dc);
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
