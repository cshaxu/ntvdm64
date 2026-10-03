#ifndef FRONTEND_WINDOW_CONTROLLER_H
#define FRONTEND_WINDOW_CONTROLLER_H
#include <windows.h>
#include "lib/kvm-window/window_interface.h"
#include "window_input_queue.h"

typedef struct frontend_window_controller frontend_window_controller;
typedef enum frontend_display_mode {
    FRONTEND_DISPLAY_CONSOLE,
    FRONTEND_DISPLAY_WINDOW
} frontend_display_mode;

typedef struct frontend_window_callbacks {
    void *context;
    /* Frontend owner thread, after FIFO dequeue. Includes UI-time key state.
     * No synchronous controller destruction/reentry from this callback. */
    lib_bool (*input)(void *,const frontend_window_input *);
    /* Frontend I/O owner: select presentation, not execution or completion. */
    DWORD (*route)(void *,BOOL window,BOOL dos_graphics);
} frontend_window_callbacks;

/* All calls except library callbacks are serialized by the frontend I/O
 * owner. The callback context outlives successful destruction. */
DWORD frontend_window_create(frontend_window_controller **,
    const frontend_window_callbacks *,const char *title);
HANDLE frontend_window_wake(frontend_window_controller *);
DWORD frontend_window_select(frontend_window_controller *,frontend_display_mode);
/* Raster carrier != DOS graphics: native Console rasters pass FALSE. */
DWORD frontend_window_present(frontend_window_controller *,const kvm_window_frame *,BOOL dos_graphics);
DWORD frontend_window_set_title(frontend_window_controller *,const char *);
DWORD frontend_window_poll(frontend_window_controller *);
/* Execution-owner handoff discards stale pixels, but retains display policy. */
DWORD frontend_window_clear(frontend_window_controller *);
DWORD frontend_window_destroy(frontend_window_controller *);
BOOL frontend_window_visible(const frontend_window_controller *);
frontend_display_mode frontend_window_mode(const frontend_window_controller *);
#endif
