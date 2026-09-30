#ifndef FRONTEND_WINDOW_MOUSE_H
#define FRONTEND_WINDOW_MOUSE_H
#include "window_input_queue.h"
#include "interface/console_mouse.h"

typedef DWORD (*frontend_mouse_sink)(void *,const INPUT_RECORD *,DWORD);
/* DOS receives relative frame pixels, never native Console positions. Guest
 * position, range, cursor shape and drawing remain original mouse owners. */
typedef struct frontend_dos_mouse {
    uint16_t width,height,buttons;
    lib_u64 source;
    BOOL active;
} frontend_dos_mouse;
DWORD frontend_dos_mouse_geometry(frontend_dos_mouse *,unsigned,unsigned);
DWORD frontend_dos_mouse_enter(frontend_dos_mouse *,frontend_mouse_sink,void *);
DWORD frontend_dos_mouse_dispatch(frontend_dos_mouse *,const frontend_window_input *,
    frontend_mouse_sink,void *);
DWORD frontend_dos_mouse_leave(frontend_dos_mouse *,frontend_mouse_sink,void *);
/* Map the captured host pointer to the native worker's text content pixels.
 * DOS never uses this mapping or receives native Console coordinates. */
BOOL frontend_native_pointer_position(const frontend_window_input *,unsigned,unsigned,
    int32_t *,int32_t *);
#endif
