#ifndef KVM_WINDOW_FRAME_INTERFACE_H
#define KVM_WINDOW_FRAME_INTERFACE_H

#include "lib/kvm-base/frame_interface.h"

#define KVM_WINDOW_GRAPHICS_MAX_WIDTH 1280u
#define KVM_WINDOW_GRAPHICS_MAX_HEIGHT 768u
#define KVM_WINDOW_GRAPHICS_MAX_PIXELS (KVM_WINDOW_GRAPHICS_MAX_WIDTH * KVM_WINDOW_GRAPHICS_MAX_HEIGHT)
#define KVM_WINDOW_GRAPHICS_PALETTE_ENTRIES 256u
#define KVM_WINDOW_DEFAULT_FONT_HEIGHT 16u
#define KVM_WINDOW_FONT_HEIGHT 32u
#define KVM_WINDOW_FONT_GLYPHS 256u
#define KVM_WINDOW_TEXT_UNDERLINE 1u

typedef struct kvm_window_text_frame {
    kvm_text_frame base;
    lib_u8 font[KVM_WINDOW_FONT_GLYPHS * KVM_WINDOW_FONT_HEIGHT];
    lib_u8 secondary_font[KVM_WINDOW_FONT_GLYPHS * KVM_WINDOW_FONT_HEIGHT];
    lib_u8 styles[KVM_TEXT_COLUMNS * KVM_TEXT_ROWS];
    lib_u8 secondary_cursor_top;
    lib_u8 secondary_cursor_bottom;
    lib_bool secondary_cursor_visible;
} kvm_window_text_frame;

typedef struct kvm_window_graphics_frame {
    lib_u32 width;
    lib_u32 height;
    lib_u32 stride;
    lib_u32 palette[KVM_WINDOW_GRAPHICS_PALETTE_ENTRIES]; /* 0x00RRGGBB */
    /* Complete image; damage is derived by the receiving Window. */
    lib_u8 pixels[KVM_WINDOW_GRAPHICS_MAX_PIXELS];
} kvm_window_graphics_frame;

typedef struct kvm_window_frame {
    lib_bool valid;
    lib_bool graphics;
    union {
        kvm_window_text_frame text;
        kvm_window_graphics_frame image;
    };
} kvm_window_frame;

static inline lib_status kvm_window_frame_validate(const kvm_window_frame *frame)
{
    lib_status status;
    if (frame == LIB_NULL || frame->valid == 0u) return LIB_STATUS_INVALID_ARGUMENT;
    if (frame->graphics != 0u) {
        if (frame->image.width == 0u || frame->image.height == 0u ||
            frame->image.stride < frame->image.width)
            return LIB_STATUS_INVALID_ARGUMENT;
        if (frame->image.width > KVM_WINDOW_GRAPHICS_MAX_WIDTH ||
            frame->image.height > KVM_WINDOW_GRAPHICS_MAX_HEIGHT ||
            frame->image.stride > KVM_WINDOW_GRAPHICS_MAX_WIDTH ||
            frame->image.height > KVM_WINDOW_GRAPHICS_MAX_PIXELS / frame->image.stride)
            return LIB_STATUS_UNSUPPORTED;
        return LIB_STATUS_OK;
    }
    status = kvm_text_frame_validate(&frame->text.base);
    if (status != LIB_STATUS_OK) return status;
    if (frame->text.base.font_height > KVM_WINDOW_FONT_HEIGHT)
        return LIB_STATUS_UNSUPPORTED;
    for (lib_size row = 0u; row < frame->text.base.text_rows; ++row)
        for (lib_size column = 0u; column < frame->text.base.text_columns; ++column)
            if (frame->text.styles[row * KVM_TEXT_COLUMNS + column] &
                ~KVM_WINDOW_TEXT_UNDERLINE) return LIB_STATUS_INVALID_ARGUMENT;
    return LIB_STATUS_OK;
}

/* Call only after validation. No inactive union arm or pixel tail is copied. */
static inline lib_size kvm_window_frame_size_bytes(const kvm_window_frame *frame)
{
    return frame->graphics != 0u ? lib_offsetof(kvm_window_frame, image.pixels) +
        (lib_size)frame->image.stride * frame->image.height :
        lib_offsetof(kvm_window_frame, text) + sizeof(frame->text);
}

static inline lib_bool kvm_window_frame_copy(kvm_window_frame *destination,
    const kvm_window_frame *source)
{
    if (destination == LIB_NULL || kvm_window_frame_validate(source) != LIB_STATUS_OK) return LIB_FALSE;
    if (destination != source)
        lib_memory_copy(destination, source, kvm_window_frame_size_bytes(source));
    return LIB_TRUE;
}

#endif
