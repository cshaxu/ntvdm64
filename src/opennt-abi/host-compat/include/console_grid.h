#ifndef OPENNT_CONSOLE_GRID_H
#define OPENNT_CONSOLE_GRID_H
#include <windows.h>
/* Stateless NT4 row-preserving binding shared by native presentation owners. */
BOOL opennt_console_resize_grid(HANDLE,const COORD *,BOOL,const SMALL_RECT *);
#endif
