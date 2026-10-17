#ifndef WOW_EDIT_DRAWING_BINDINGS_H
#define WOW_EDIT_DRAWING_BINDINGS_H
#include <windows.h>

/* NT4 winuserp.h palette extent. Do not grow DF_GRAY with the host palette. */
#define WOW_EDIT_COLOR_COUNT (COLOR_INFOBK + 1)
extern HBRUSH wow_edit_system_brushes[WOW_EDIT_COLOR_COUNT];
extern HBRUSH wow_edit_gray_brush;
BOOL WINAPI wow_edit_drawing_ready(void);
/* OPENNT-HOST-097 original /Gz entry, requires the captured control context. */
LONG WINAPI TabTextOut(HDC, int, int, LPCWSTR, int, int, LPINT, int, BOOL);
#endif
