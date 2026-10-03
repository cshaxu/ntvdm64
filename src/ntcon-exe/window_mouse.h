#ifndef FRONTEND_WINDOW_MOUSE_H
#define FRONTEND_WINDOW_MOUSE_H
#include "window_input_queue.h"
#include "common/protocol/console_mouse.h"

typedef DWORD (*frontend_mouse_sink)(void *,const INPUT_RECORD *,DWORD);
/* Both workers receive relative frame pixels. Guest/native position, range,
 * cursor shape and drawing remain worker-owned. */
typedef struct frontend_window_mouse {
    uint16_t width,height,buttons;
    lib_u64 source;
    BOOL active;
} frontend_window_mouse;
DWORD frontend_window_mouse_geometry(frontend_window_mouse *,unsigned,unsigned);
DWORD frontend_window_mouse_enter(frontend_window_mouse *,frontend_mouse_sink,void *);
DWORD frontend_window_mouse_dispatch(frontend_window_mouse *,const frontend_window_input *,
    frontend_mouse_sink,void *);
DWORD frontend_window_mouse_leave(frontend_window_mouse *,frontend_mouse_sink,void *);
#endif
