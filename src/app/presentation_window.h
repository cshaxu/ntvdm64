#ifndef APP_PRESENTATION_WINDOW_H
#define APP_PRESENTATION_WINDOW_H

#include <stdint.h>
#include <windows.h>

#include "session/session.h"

#define APP_PRESENTATION_WINDOW_VERSION UINT32_C(1)
#define APP_PRESENTATION_GLYPH_WIDTH 8u
#define APP_PRESENTATION_GLYPH_HEIGHT 16u
#define APP_PRESENTATION_GLYPH_COUNT 256u
#define APP_PRESENTATION_FONT_BYTES \
    (APP_PRESENTATION_GLYPH_COUNT * APP_PRESENTATION_GLYPH_HEIGHT)

enum app_presentation_window_state {
    APP_PRESENTATION_WINDOW_READY = 0u,
    APP_PRESENTATION_WINDOW_STARTING = 1u,
    APP_PRESENTATION_WINDOW_OPEN = 2u,
    APP_PRESENTATION_WINDOW_FAILED = 3u,
    APP_PRESENTATION_WINDOW_CLOSING = 4u,
    APP_PRESENTATION_WINDOW_CLOSED = 5u
};

typedef struct app_presentation_window {
    uint32_t abi_version;
    uint32_t struct_bytes;
    session *owner;
    HANDLE ready_event;
    HANDLE closed_event;
    HANDLE thread;
    HWND window;
    DWORD thread_id;
    volatile LONG state;
    volatile LONG fullscreen;
    volatile LONG repaint_pending;
    RECT windowed_rect;
    LONG_PTR windowed_style;
    HANDLE input;
    /* A copied snapshot supplied by the SoftPC presentation boundary. App
     * never aliases a ROM, VGA aperture, or source-owned font pointer. */
    uint8_t current_font[APP_PRESENTATION_FONT_BYTES];
    uint32_t current_font_loaded;
} app_presentation_window;

void app_presentation_window_initialize(app_presentation_window *window);
int app_presentation_window_valid(const app_presentation_window *window);
/* Prepare and open are deliberately separate: the sink binds while session is
 * ready, but source-owned graphics or an explicit host display gesture opens
 * the UI only after original host startup has activated the session. */
int app_presentation_window_prepare(app_presentation_window *window,
    session *owner);
int app_presentation_window_open(app_presentation_window *window);
int app_presentation_window_close(app_presentation_window *window);
int app_presentation_window_active(const app_presentation_window *window);

#endif
