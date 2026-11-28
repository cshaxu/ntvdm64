#ifndef WOW_EDIT_DRAWING_FIXTURE_PRECOMP_H
#define WOW_EDIT_DRAWING_FIXTURE_PRECOMP_H
#include <windows.h>
#include "non-mvdm/windows/core/w32inc/w32gdip.h"
/* Original winuserp.h color range, not a modern extended palette. */
#define COLOR_MAX (COLOR_INFOBK + 1)
#include "compat/include/opennt_user_multipass.h"

/* Test-only finite GDI boundary. Compile the two original RTL units with
 * this directory first in their include path; never use in product builds. */
extern HBRUSH ahbrSystem[COLOR_MAX], ghbrGray;
extern int drawing_fixture_CXBORDER, drawing_fixture_CYBORDER;
#define SYSMET(index) drawing_fixture_##index
BOOL WINAPI drawing_fixture_poly(HDC, DWORD, PPOLYPATBLT, DWORD, DWORD);
#define UserPolyPatBlt drawing_fixture_poly
BOOL WINAPI DrawFrame(HDC, PRECT, int, int);
BOOL APIENTRY CopyInflateRect(LPRECT, CONST RECT *, int, int);
extern struct drawing_display { UINT BitCount; } drawing_display;
extern COLORREF drawing_colors[COLOR_MAX];
#define oemInfo drawing_display
#define SYSRGB(index) drawing_colors[COLOR_##index]
#define SYSHBR(index) ahbrSystem[COLOR_##index]
BOOL WINAPI drawing_edge(HDC, LPRECT, UINT, UINT);
int WINAPI drawing_fill(HDC, CONST RECT *, HBRUSH);
COLORREF WINAPI drawing_back(HDC, COLORREF);
COLORREF WINAPI drawing_fore(HDC, COLORREF);
#define DrawEdge drawing_edge
#define FillRect drawing_fill
#define UserSetBkColor drawing_back
#define UserSetTextColor drawing_fore
VOID WINAPI DrawPushButton(HDC, LPRECT, UINT, UINT);
/* Public GDI leaves for the restored original drawing helpers. */
#define UserSelectBrush(dc, brush) ((HBRUSH)SelectObject((dc), (brush)))
#define UserGetViewportExtEx GetViewportExtEx
#define UserGetWindowExtEx GetWindowExtEx
#define ghbrWhite ((HBRUSH)GetStockObject(WHITE_BRUSH))
#define ghbrBlack ((HBRUSH)GetStockObject(BLACK_BRUSH))
#define UserTextOutW TextOutW
BOOL WINAPI GetSignFromMappingMode(HDC, PPOINT);
/* XP drawing names retain the same fixture-owned metric/color resources. */
#undef SYSMETRTL
#define SYSMETRTL(index) drawing_fixture_##index
#undef SYSRGBRTL
#define SYSRGBRTL(index) drawing_colors[COLOR_##index]
#undef SYSHBRUSH
#define SYSHBRUSH(index) ahbrSystem[index]
#define UserGetLayout GetLayout
#define UserSetGraphicsMode SetGraphicsMode
#define UserGetTextAlign GetTextAlign
#define UserSetTextAlign SetTextAlign
#define UserAssert(condition) ((void)0)
/* XP windows/published/winuser.w:2998. */
#define DFCS_CAPTIONALL 0x000F
#endif
