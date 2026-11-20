#ifndef NTVDM_CONSOLE_GRAPHICS_H
#define NTVDM_CONSOLE_GRAPHICS_H
#include <windows.h>
#include "publication.h"
#include "console_video.h"
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
int ntvdm_console_graphics_palette(HANDLE,HPALETTE,DWORD);
/* TRUE only for this session's live graphics output; count may be negative. */
BOOL ntvdm_console_graphics_cursor(HANDLE,BOOL,int *);
/* Original Video 7 hardware-pointer callback; pattern remains caller-owned. */
void ntvdm_console_graphics_v7_pointer(const BYTE *,unsigned,unsigned,BOOL);
void ntvdm_console_graphics_begin_update(void);
void ntvdm_console_graphics_end_update(void);
ntvdm_console_graphics *ntvdm_console_graphics_context(void);
/* NULL description retires graphics for a text transition. Called only after
 * releasing the painter mutex; transport serializes the complete frame. */
BOOL ntvdm_console_publish_video(const console_video_description *,const void *,size_t);
/* Guest owner toggles publication at route and final-paint boundaries. */
BOOL ntvdm_console_video_async(BOOL);
#endif
