#ifndef NTVDM_CONSOLE_VIDEO_H
#define NTVDM_CONSOLE_VIDEO_H
#include <stdint.h>

/* Copied top-down packed DIB. Palette values are 0x00RRGGBB, not HPALETTE,
 * RGBQUAD pointers or a library-specific KVM frame. The existing authenticated
 * Console channel carries BEGIN, contiguous DATA chunks, and TEXT retirement.
 * A monotonically increasing nonzero serial identifies each frame or TEXT
 * transition. Only a complete frame becomes visible to the frontend owner. */
typedef struct console_video_description {
    uint32_t width, height, stride, depth, bytes;
    uint32_t palette[256];
    uint32_t kind;
} console_video_description;
enum { CONSOLE_VIDEO_DIB=0, CONSOLE_VIDEO_TEXT_FRAME=1 };
/* TEXT_FRAME payload: this fixed header, followed by height tightly packed
 * rows of width original glyph/attribute byte pairs. No library/native types.
 * description.depth is zero; stride is width*2. Complete publication only. */
typedef struct console_text_style {
    uint32_t font_height,attribute_font_select;
    int32_t cursor_column,cursor_row,cursor_start,cursor_height;
    int32_t cursor_start1,cursor_height1;
    uint32_t cursor_visible;
    uint8_t fonts[2][256][32];
} console_text_style;
#endif
