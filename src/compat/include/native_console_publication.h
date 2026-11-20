#ifndef COMPAT_NATIVE_CONSOLE_PUBLICATION_H
#define COMPAT_NATIVE_CONSOLE_PUBLICATION_H

#include "console_transport.h"
#include "publication.h"
#include "native_console_text_frame.h"

typedef struct ntvwm_presentation ntvwm_presentation;

/* Native capture owns acquisition. Publisher owns this immutable complete
 * transaction; Unicode is not reconstructed from the PC-glyph frame. */
typedef struct native_publication {
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    CONSOLE_CURSOR_INFO cursor;
    console_video_description description;
    DWORD total;
    BOOL title_read;
    char title[CONSOLE_IO_TITLE_BYTES];
    /* Followed by total CHAR_INFO cells, then description.bytes frame bytes. */
} native_publication;
struct ntvwm_presentation {
    ntcon_worker_client channel;
    CRITICAL_SECTION lock;
    /* A polling sample is not a display event. Keep the last source-owned
     * native state separately from published_cells, which is only the last
     * acknowledged row image used by publish_copy to avoid redundant writes.
     * This cache is deliberately here, not in worker-base or NTCON: explicit
     * text calls below must still be sent exactly as requested. */
    native_publication *captured;
    SIZE_T captured_bytes;
    CHAR_INFO *published_cells;
    DWORD published_count,published_width;
    worker_base_publication *publisher;
    console_text_style handoff_font;
    BOOL has_handoff_font;
    char published_title[CONSOLE_IO_TITLE_BYTES];
    BOOL title_valid;
    ntvwm_mouse mouse;
};
/* Serialized requests and immutable-frame delivery for this endpoint. */
DWORD ntvwm_presentation_exchange(ntvwm_presentation *, console_io_request *, console_io_reply *);
DWORD ntvwm_publish_copy(void *, const void *, SIZE_T);
/* Consume a completed screen image while serialized against publication. */
void ntvwm_publication_set_cells(ntvwm_presentation *, CHAR_INFO *, DWORD count, DWORD width);
DWORD ntvwm_presentation_text(ntvwm_presentation *, const console_video_description *,
    const void *, SIZE_T);

#endif
