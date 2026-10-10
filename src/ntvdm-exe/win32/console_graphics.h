#ifndef NTVDM_CONSOLE_GRAPHICS_H
#define NTVDM_CONSOLE_GRAPHICS_H
#include <windows.h>
#include "worker-base/publication.h"
typedef struct ntvdm_console_graphics ntvdm_console_graphics;
ntvdm_console_graphics *ntvdm_console_graphics_create(void);
void ntvdm_console_graphics_destroy(ntvdm_console_graphics *);
/* The common publisher owns cadence and shutdown; this adapter owns source
 * dirtiness and DIB capture. Attach before either side becomes active. */
DWORD ntvdm_console_graphics_attach_publisher(ntvdm_console_graphics *,worker_base_publication *);
/* Returns zero for non-owned handles; otherwise the source-shaped result. */
int ntvdm_console_graphics_invalidate(HANDLE,const SMALL_RECT *);
/* Original host flush completed a software-VGA paint.  The copied frontend
 * needs one source-owned publication request even when the legacy physical
 * fullscreen renderer would have displayed that paint directly. */
int ntvdm_console_graphics_flush(void);
/* Test observers may replace this no-op probe at link time.  It deliberately
 * carries only the original host-flush state, never a presentation request. */
void ntvdm_console_graphics_flush_probe(DWORD screen_state,DWORD mode_type);
int ntvdm_console_graphics_palette(HANDLE,HPALETTE,DWORD);
/* TRUE only for this session's live graphics output; count may be negative. */
BOOL ntvdm_console_graphics_cursor(HANDLE,BOOL,int *);
#endif
