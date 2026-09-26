#ifndef NTVDM_CONSOLE_GRAPHICS_H
#define NTVDM_CONSOLE_GRAPHICS_H
#include <windows.h>
typedef struct ntvdm_console_graphics ntvdm_console_graphics;
ntvdm_console_graphics *ntvdm_console_graphics_create(void);
void ntvdm_console_graphics_destroy(ntvdm_console_graphics *);
/* Returns zero for non-owned handles; otherwise the source-shaped result. */
int ntvdm_console_graphics_invalidate(HANDLE,const SMALL_RECT *);
int ntvdm_console_graphics_palette(HANDLE,HPALETTE,DWORD);
/* TRUE only for this session's live graphics output; count may be negative. */
BOOL ntvdm_console_graphics_cursor(HANDLE,BOOL,int *);
#endif
