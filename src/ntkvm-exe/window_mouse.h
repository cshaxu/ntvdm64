#ifndef FRONTEND_WINDOW_MOUSE_H
#define FRONTEND_WINDOW_MOUSE_H
#include "window_input_queue.h"
#include "product-abi/console_mouse.h"

/* Native Console only. DOS virtual coordinates and guest mouse state remain
 * owned by original nt_mouse.c, not this presentation converter. */
typedef struct frontend_native_mouse {
    SMALL_RECT viewport;
    unsigned cell_width,cell_height;
    LONG x,y;
    DWORD buttons;
    lib_u64 source;
    BOOL ready;
} frontend_native_mouse;
typedef DWORD (*frontend_mouse_sink)(void *,const INPUT_RECORD *,DWORD);
DWORD frontend_native_mouse_geometry(frontend_native_mouse *,SMALL_RECT,unsigned,unsigned);
DWORD frontend_native_mouse_dispatch(frontend_native_mouse *,const frontend_window_input *,
    frontend_mouse_sink,void *);
/* Release to the old consumer before changing consumer identity. A sink
 * failure leaves the ledger unchanged; callers must not finish the handoff. */
DWORD frontend_native_mouse_release(frontend_native_mouse *,BOOL,frontend_mouse_sink,void *);

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
#endif
