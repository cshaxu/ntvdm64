/* The original V7 controller owns the registers and selects the pattern.
 * Bind its existing host callbacks to the worker-owned indexed bitmap. */
#include <windows.h>
#include "insignia.h"
#include "host_def.h"
#include "xt.h"
#include "gvi.h"
#include "gmi.h"
#include "gfx_upd.h"
#include "egacpu.h"
#include "egaports.h"
#include "ntvdm-exe/win32/console_graphics.h"

static void paint_pointer(sys_addr pattern, word x, word y)
{
    if (!EGA_planes || pattern > (4u * EGA_PLANE_SIZE) - 256u) return;
    ntvdm_console_graphics_v7_pointer(EGA_planes+pattern,x,y,TRUE);
}

static void clear_pointer(word x, word y)
{
    (void)x;(void)y;
    ntvdm_console_graphics_v7_pointer(NULL,0,0,FALSE);
}

void mvdm_softpc_v7_pointer_attach(void)
{
    paint_v7ptr=paint_pointer;
    clear_v7ptr=clear_pointer;
}
