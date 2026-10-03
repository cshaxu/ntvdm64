#ifndef FRONTEND_TEXT_FRAME_H
#define FRONTEND_TEXT_FRAME_H

#include <windows.h>
#include "lib/kvm-window/frame_interface.h"

/* Local copied-frame metadata; no added cross-process wire fields. */
typedef struct frontend_text_extension {
    lib_u8 cursor_top,cursor_bottom,cursor_visible;
    const lib_u8 *cell_styles;
    size_t cell_style_stride;
} frontend_text_extension;

typedef struct frontend_text_snapshot {
    kvm_window_text_frame fonts;
    frontend_text_extension extension;
    kvm_text_cell cells[KVM_TEXT_COLUMNS*KVM_TEXT_ROWS];
    size_t cell_count;
} frontend_text_snapshot;

BOOL frontend_text_frame_prepare(const kvm_window_text_frame *fonts,
    const kvm_text_cell *cells,size_t cell_count,
    const frontend_text_extension *extension,BOOL cursor_phase,
    kvm_window_frame *output);

#endif
