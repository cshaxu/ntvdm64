#include "text_frame.h"
#include "lib/kvm-window/render.h"
#include "interface/console_video.h"

BOOL frontend_text_frame_prepare(frontend_text_raster *scratch,
    const kvm_window_text_frame *fonts, const kvm_text_cell *cells,
    size_t cell_count, const frontend_text_extension *extension,
    BOOL cursor_phase, kvm_window_frame *output)
{
    unsigned row, columns, rows;
    if (output) output->valid = 0;
    if (!fonts || !cells || !output) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    columns = fonts->base.text_columns;
    rows = fonts->base.text_rows;
    if (columns > KVM_TEXT_COLUMNS || rows > KVM_TEXT_ROWS ||
        fonts->base.font_height > KVM_WINDOW_FONT_HEIGHT ||
        (extension && (extension->cursor_visible || extension->cell_styles)))
        return frontend_text_frame_rasterize(scratch, fonts, cells, cell_count,
            extension, cursor_phase, output);
    if (cell_count < (size_t)columns * rows) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER); return FALSE;
    }
    output->graphics = 0;
    output->text = *fonts;
    output->text.base.cursor_phase = (lib_u8)!!cursor_phase;
    for (row = 0; row < rows; ++row)
        memcpy(output->text.base.cells + row * KVM_TEXT_COLUMNS,
            cells + row * columns, columns * sizeof(*cells));
    if (kvm_text_frame_validate(&output->text.base) != LIB_STATUS_OK) {
        SetLastError(ERROR_INVALID_DATA); return FALSE;
    }
    output->valid = 1;
    return TRUE;
}

static BOOL cursor_slice(kvm_window_frame *tile, const kvm_window_rect *display,
    unsigned top, unsigned bottom, BOOL visible, unsigned offset, unsigned height,
    kvm_window_rect *cursor)
{
    unsigned end = bottom + 1;
    if (bottom < top) { top = 0; end = 32; }
    if (!visible || top >= offset + height || end <= offset) return FALSE;
    tile->text.base.cursor_visible = TRUE;
    tile->text.base.cursor_top = (lib_u8)(top > offset ? top - offset : 0);
    tile->text.base.cursor_bottom = (lib_u8)((end < offset + height ? end - offset : height) - 1);
    return kvm_window_cursor_rect(tile, display, cursor);
}

BOOL frontend_text_frame_rasterize(frontend_text_raster *scratch,
    const kvm_window_text_frame *fonts, const kvm_text_cell *cells,
    size_t cell_count, const frontend_text_extension *extension,
    BOOL cursor_phase, kvm_window_frame *output)
{
    lib_u32 columns, rows, height, row, first, index, offset;
    if (output) output->valid = 0;
    if (!scratch || !fonts || !cells || !output || output == &scratch->tile) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    columns = fonts->base.text_columns;
    rows = fonts->base.text_rows;
    height = fonts->base.font_height ? fonts->base.font_height : KVM_WINDOW_FONT_HEIGHT;
    if (!columns || !rows || height > 32 || (height > 16 && !extension) ||
        columns > KVM_WINDOW_GRAPHICS_MAX_WIDTH / 8u ||
        rows > KVM_WINDOW_GRAPHICS_MAX_HEIGHT / height) {
        SetLastError(ERROR_NOT_SUPPORTED); return FALSE;
    }
    if (cell_count < (size_t)columns * rows) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER); return FALSE;
    }
    scratch->tile.valid = 1;
    scratch->tile.graphics = 0;
    scratch->tile.text = *fonts;
    scratch->tile.text.base.text_rows = 1;
    for (index = 0; index < 16; ++index) {
        output->image.palette[index] = fonts->base.text_palette[index] & 0xffffffu;
        output->image.palette[index + 16] = output->image.palette[index] ^ 0xffffffu;
        /* Render indices with the unchanged library rather than duplicating
         * its glyph-bank, font-height and bit-order algorithms. */
        scratch->tile.text.base.text_palette[index] = index;
    }
    output->graphics = 1;
    output->image.width = output->image.stride = columns * 8u;
    output->image.height = rows * height;
    for (offset = 0; offset < height; offset += 16) {
    lib_u32 slice_height = height - offset;
    if (slice_height > 16) slice_height = 16;
    scratch->tile.text.base.font_height = slice_height;
    memcpy(scratch->tile.text.font, offset ? extension->upper_font[0] : fonts->font,
        sizeof(fonts->font));
    memcpy(scratch->tile.text.secondary_font, offset ? extension->upper_font[1] : fonts->secondary_font,
        sizeof(fonts->secondary_font));
    for (row = 0; row < rows; ++row) for (first = 0; first < columns; first += KVM_TEXT_COLUMNS) {
        lib_u32 count = columns - first, x, y;
        lib_bool valid = LIB_FALSE;
        kvm_window_rect damage, display, cursor, cursor2;
        BOOL draw_cursor, draw_cursor2;
        if (count > KVM_TEXT_COLUMNS) count = KVM_TEXT_COLUMNS;
        scratch->tile.text.base.text_columns = (lib_u16)count;
        memcpy(scratch->tile.text.base.cells, cells + (size_t)row * columns + first,
            count * sizeof(*cells));
        if (!kvm_window_render_frame(&scratch->tile, scratch->pixels, count * 8u,
                slice_height, &valid, &damage)) {
            SetLastError(ERROR_INVALID_DATA); return FALSE;
        }
        display.left = display.top = 0;
        display.right = (lib_i32)count * 8;
        display.bottom = (lib_i32)slice_height;
        scratch->tile.text.base.cursor_row = fonts->base.cursor_row == (lib_i32)row ? 0 : -1;
        scratch->tile.text.base.cursor_column = fonts->base.cursor_column >= 0 &&
            (lib_u32)fonts->base.cursor_column >= first &&
            (lib_u32)fonts->base.cursor_column < first + count ?
            fonts->base.cursor_column - (lib_i32)first : -1;
        draw_cursor = cursor_phase && cursor_slice(&scratch->tile, &display,
            fonts->base.cursor_top, fonts->base.cursor_bottom, fonts->base.cursor_visible,
            offset, slice_height, &cursor);
        draw_cursor2 = cursor_phase && extension && cursor_slice(&scratch->tile, &display,
            extension->cursor_top, extension->cursor_bottom, extension->cursor_visible,
            offset, slice_height, &cursor2);
        for (y = 0; y < slice_height; ++y) for (x = 0; x < count * 8u; ++x) {
            lib_u8 colour = (lib_u8)scratch->pixels[y * count * 8u + x];
            if(extension && extension->cell_styles) {
                size_t cell=(size_t)row*columns+first+x/8;
                lib_u8 flags=extension->cell_styles[cell*extension->cell_style_stride];
                if((flags&CONSOLE_TEXT_UNDERLINE) && offset+y==height-1)
                    colour=cells[cell].foreground;
            }
            if ((draw_cursor && (lib_i32)x >= cursor.left && (lib_i32)x < cursor.right &&
                (lib_i32)y >= cursor.top && (lib_i32)y < cursor.bottom) ||
                (draw_cursor2 && (lib_i32)x >= cursor2.left && (lib_i32)x < cursor2.right &&
                (lib_i32)y >= cursor2.top && (lib_i32)y < cursor2.bottom)) colour += 16;
            output->image.pixels[(row * height + offset + y) * output->image.stride + first * 8u + x] = colour;
        }
    }
    }
    output->valid = 1;
    return TRUE;
}
