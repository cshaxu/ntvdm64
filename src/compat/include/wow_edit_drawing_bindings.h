#ifndef WOW_EDIT_DRAWING_BINDINGS_H
#define WOW_EDIT_DRAWING_BINDINGS_H
#include <windows.h>

#include "compat/include/wow_control_brushes.h"
/* OPENNT-HOST-097 original /Gz entry, requires the captured control context. */
LONG WINAPI TabTextOut(HDC, int, int, LPCWSTR, int, int, LPINT, int, BOOL);
#endif
