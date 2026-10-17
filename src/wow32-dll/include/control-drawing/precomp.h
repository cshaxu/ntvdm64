#ifndef WOW_CONTROL_DRAWING_PRECOMP_H
#define WOW_CONTROL_DRAWING_PRECOMP_H
#include "wow_edit_drawing_bindings.h"
#include "wow_edit_native_bindings.h"
#include "wow_edit_entry_bindings.h"
#define COLOR_MAX WOW_EDIT_COLOR_COUNT
#include "opennt-host/windows/core/ntuser/inc/user.h"

/* OPENNT-HOST-077: resource/metric leaves only. Original DrawFrame retains
 * its geometry, raster operation, brush selection and return behavior. */
#define ahbrSystem wow_edit_system_brushes
#define ghbrGray wow_edit_gray_brush
#define SYSMET(index) GetSystemMetrics(SM_##index)
#define UserPolyPatBlt wow_edit_native_PolyPatBlt
/* OPENNT-HOST-097: original TabTextOut and its font/mapping predicate. */
#define gpsi wow_edit_current
#define GdiGetCharDimensions wow_edit_native_GdiGetCharDimensions
#define UserGetHFONT(dc) ((HFONT)GetCurrentObject((dc), OBJ_FONT))
#define UserGetMapMode GetMapMode
#define ghFontSys ((HFONT)GetStockObject(SYSTEM_FONT))
#define RIPMSG0(level, text) ((void)0)
BOOL WINAPI IsSysFontAndDefaultMode(HDC);
/* OPENNT-HOST-110: original user.h contract for the retained text body. */
#define CH_PREFIX TEXT('&')
LONG GetPrefixCount(LPCWSTR lpstr, int cb, LPWSTR lpstrCopy, int cbCopy);
/* OPENNT-HOST-112: reuse the already selected RTL allocation service. */
PVOID WINAPI OpenNtUserRtlAllocMem(SIZE_T);
VOID WINAPI OpenNtUserRtlFreeMem(PVOID);
#define UserRtlAllocMem OpenNtUserRtlAllocMem
#define UserRtlFreeMem OpenNtUserRtlFreeMem
#define UserGetTextExtentPointW GetTextExtentPointW
BOOL WINAPI PSMGetTextExtent(HDC, LPWSTR, int, PSIZE);
#ifdef WOW_ORIGINAL_MNEMONIC_DRAWING
/* OPENNT-HOST-115: original text/underline algorithm, public GDI leaves. */
#define UserTextOutW TextOutW
#define UserGetTextMetricsW GetTextMetricsW
#define UserGetTextColor GetTextColor
#define UserExtTextOutW ExtTextOutW
#define UserSetBkColor SetBkColor
VOID WINAPI PSMTextOut(HDC, int, int, LPWSTR, int);
#endif
#ifdef WOW_ORIGINAL_BUTTON_DRAWING
/* OPENNT-HOST-111: original arithmetic; native resources/scoped input only. */
#define oemInfo (wow_edit_current->screen)
#define SYSRGB(index) GetSysColor(COLOR_##index)
#define SYSHBR(index) ahbrSystem[COLOR_##index]
#define UserSetBkColor SetBkColor
#define UserSetTextColor SetTextColor
VOID WINAPI DrawPushButton(HDC, LPRECT, UINT, UINT);
#endif
#endif
