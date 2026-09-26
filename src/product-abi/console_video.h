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
} console_video_description;
#endif
