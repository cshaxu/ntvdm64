/* The original V7 controller owns the registers and selects the pattern.
 * Bind its existing host callbacks to the worker-owned indexed bitmap. */
#include <windows.h>
#include "ntvdm-exe/win32/console_graphics.h"
#include "v7_selected_plane.h"

/* Selected V7 ABI from egaports.h and gfx_upd.h. The original VGA device
 * owns all four planes; the adapter holds no copy of their state. */
extern unsigned char *EGA_planes;
extern void (*paint_v7ptr)();
extern void (*clear_v7ptr)();

static void paint_pointer(unsigned long pattern, unsigned short x, unsigned short y)
{
    if (!EGA_planes || pattern > (4u * EGA_PLANE_SIZE) - 256u) return;
    ntvdm_console_graphics_v7_pointer(EGA_planes+pattern,x,y,TRUE);
}

static void clear_pointer(unsigned short x, unsigned short y)
{
    (void)x;(void)y;
    ntvdm_console_graphics_v7_pointer(NULL,0,0,FALSE);
}

void mvdm_softpc_v7_pointer_attach(void)
{
    paint_v7ptr=paint_pointer;
    clear_v7ptr=clear_pointer;
}
