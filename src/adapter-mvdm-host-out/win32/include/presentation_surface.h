#ifndef MVDM_PRESENTATION_SURFACE_H
#define MVDM_PRESENTATION_SURFACE_H

#include <stdint.h>

#include "session/session.h"

/* App-facing copies of the source-owned presentation plane.  The adapter
 * resolves and waits on the source mutex internally; neither native handles
 * nor writable pointers cross this boundary. */
int mvdm_presentation_graphics_describe(session *owner, uint32_t *width_out,
    uint32_t *height_out, uint32_t *bits_per_pixel_out, uint32_t *stride_out,
    uint32_t *bytes_out);
int mvdm_presentation_graphics_snapshot(session *owner, uint8_t *bytes,
    uint32_t capacity, uint32_t *width_out, uint32_t *height_out,
    uint32_t *bits_per_pixel_out, uint32_t *stride_out, uint32_t *bytes_out,
    uint32_t *palette, uint32_t palette_capacity,
    uint32_t *palette_entries_out);

#endif
