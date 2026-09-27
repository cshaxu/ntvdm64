#ifndef FRONTEND_TEXT_FRAME_H
#define FRONTEND_TEXT_FRAME_H

#include <windows.h>
#include "lib/kvm-window/frame_interface.h"

/* Largest copied cell grid admitted by the pixel carrier at 8x8. Extents
 * remain checked against actual font height before rendering. */
typedef struct frontend_text_extension {
    lib_u8 upper_font[2][256 * 16];
    lib_u8 cursor_top, cursor_bottom, cursor_visible;
} frontend_text_extension;

typedef struct frontend_text_snapshot {
    kvm_window_text_frame fonts;
    frontend_text_extension extension;
    kvm_text_cell cells[(KVM_WINDOW_GRAPHICS_MAX_WIDTH / 8u) *
        (KVM_WINDOW_GRAPHICS_MAX_HEIGHT / 8u)];
    size_t cell_count;
} frontend_text_snapshot;

/* Reusable frontend-owned scratch, allocated outside the stack. The source is a
 * copied painter snapshot, never a live guest/video pointer. No library change
 * is needed for text modes beyond the library's text/font/cursor carrier. */
typedef struct frontend_text_raster {
    kvm_window_frame tile;
    lib_u32 pixels[KVM_TEXT_COLUMNS * 8u * KVM_WINDOW_FONT_HEIGHT];
} frontend_text_raster;

/* fonts.base supplies dimensions, palette and cursor; cells is a tightly
 * packed complete grid, not fonts.base.cells. Payload is indexed pixels, but
 * the caller must retain TEXT as the guest frame type for display policy. */
BOOL frontend_text_frame_rasterize(frontend_text_raster *scratch,
    const kvm_window_text_frame *fonts, const kvm_text_cell *cells,
    size_t cell_count, const frontend_text_extension *extension,
    BOOL cursor_phase, kvm_window_frame *output);

/* Prefer the library's native text frame. Raster fallback is only for extents,
 * tall glyphs or a second cursor range that its text contract cannot carry. */
BOOL frontend_text_frame_prepare(frontend_text_raster *scratch,
    const kvm_window_text_frame *fonts, const kvm_text_cell *cells,
    size_t cell_count, const frontend_text_extension *extension,
    BOOL cursor_phase, kvm_window_frame *output);

#endif
