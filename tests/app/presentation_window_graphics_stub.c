#include "adapter-mvdm-host-out/win32/include/presentation_surface.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_presentation_font.h"

/* The toggle fixture validates app/session surface lifetime only.  It has no
 * original graphicsResize producer, so force the app down its text-surface
 * fallback without introducing a second product implementation. */
int mvdm_presentation_graphics_describe(session *owner, uint32_t *width_out,
    uint32_t *height_out, uint32_t *bits_out, uint32_t *stride_out,
    uint32_t *bytes_out)
{
    (void)owner;
    (void)width_out;
    (void)height_out;
    (void)bits_out;
    (void)stride_out;
    (void)bytes_out;
    return 0;
}

int mvdm_presentation_graphics_snapshot(session *owner, uint8_t *bytes,
    uint32_t capacity, uint32_t *width_out, uint32_t *height_out,
    uint32_t *bits_out, uint32_t *stride_out, uint32_t *bytes_out,
    uint32_t *palette, uint32_t palette_capacity, uint32_t *entries_out)
{
    (void)owner;
    (void)bytes;
    (void)capacity;
    (void)width_out;
    (void)height_out;
    (void)bits_out;
    (void)stride_out;
    (void)bytes_out;
    (void)palette;
    (void)palette_capacity;
    (void)entries_out;
    return 0;
}

int mvdm_softpc_presentation_font_snapshot(session *owner, uint8_t *bytes,
    uint32_t capacity)
{
    uint32_t index;
    if (owner == NULL || bytes == NULL ||
        capacity != MVDM_SOFTPC_PRESENTATION_FONT_BYTES) return 0;
    for (index = 0u; index < capacity; ++index) bytes[index] = 0u;
    return 1;
}
