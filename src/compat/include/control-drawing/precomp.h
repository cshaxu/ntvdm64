#ifndef WOW_CONTROL_DRAWING_PRECOMP_H
#define WOW_CONTROL_DRAWING_PRECOMP_H
#include "wow_edit_drawing_bindings.h"
#include "wow_edit_native_bindings.h"
#include "non-mvdm/windows/core/w32inc/w32gdip.h"
#include "non-mvdm/public/internal/windows/inc/wingdip.h"
#include "wow_edit_entry_bindings.h"
#define COLOR_MAX WOW_EDIT_COLOR_COUNT
#include "compat/include/opennt_user_multipass.h"

/* OPENNT-HOST-077: resource/metric leaves only. Original DrawFrame retains
 * its geometry, raster operation, brush selection and return behavior. */
#define ahbrSystem wow_edit_system_brushes
#define ghbrGray wow_edit_gray_brush
#define SYSMET(index) GetSystemMetrics(SM_##index)
#define UserPolyPatBlt wow_edit_native_PolyPatBlt
/* OPENNT-HOST-097: original TabTextOut and its font/mapping predicate. */
#define gpsi wow_edit_current
#define GdiGetCharDimensions wow_edit_native_GdiGetCharDimensions
#define UserGetCharDimensionsW GdiGetCharDimensions
#define UserGetHFONT(dc) ((HFONT)GetCurrentObject((dc), OBJ_FONT))
#define UserGetMapMode GetMapMode
#define ghFontSys ((HFONT)GetStockObject(SYSTEM_FONT))
#define RIPMSG0(level, text) ((void)0)
BOOL WINAPI IsSysFontAndDefaultMode(HDC);
/* OPENNT-HOST-110: original user.h contract for the retained text body. */
#define CH_PREFIX TEXT('&')
/* OPENNT-HOST-112: reuse the already selected RTL allocation service. */
#include "compat/include/text-measure/precomp.h"
/* OPENNT-HOST-115: original text/underline algorithm, public GDI leaves. */
#define UserTextOutW TextOutW
#define UserGetTextMetricsW GetTextMetricsW
#define UserGetTextColor GetTextColor
#define UserExtTextOutW ExtTextOutW
#define UserSetBkColor SetBkColor
VOID WINAPI PSMTextOut(HDC, int, int, LPWSTR, int, DWORD);
/* OPENNT-HOST-111: original arithmetic; native resources/scoped input only. */
#define oemInfo (wow_edit_current->screen)
#define SYSRGB(index) GetSysColor(COLOR_##index)
#define SYSHBR(index) ahbrSystem[COLOR_##index]
#define UserSetBkColor SetBkColor
#define UserSetTextColor SetTextColor
VOID WINAPI DrawPushButton(HDC, LPRECT, UINT, UINT);
/* Public GDI leaves for the restored original drawing helpers. */
#define UserSelectBrush(dc, brush) ((HBRUSH)SelectObject((dc), (brush)))
#define UserGetViewportExtEx GetViewportExtEx
#define UserGetWindowExtEx GetWindowExtEx
#define ghbrWhite ((HBRUSH)GetStockObject(WHITE_BRUSH))
#define ghbrBlack ((HBRUSH)GetStockObject(BLACK_BRUSH))
/* XP client/usercli.h and rtl/userrtl.h text-provider ABI. */
#include "wow_multibyte.h"
#include "wow_control_resource_abi.h"
#include "wow_task_init_bindings.h"
#include "wow_edit_call_bindings.h"
#include "wow_user_globals_binding.h"
#define PCLIENTINFO const wow_task_client_info *
#define GetClientInfo() wow_edit_client_info()
#define TIF_16BIT WOW_TASK_TIF_16BIT
#define IS_DBCS_ENABLED() GetSystemMetrics(SM_DBCSENABLED)
#define CH_ENGLISHPREFIX 0x1E
#define CH_KANJIPREFIX 0x1F
#define USER_AWCONV_COUNTSTRINGSZ (-2)
/* XP user.h:646, usercli.h:1140 and rtl/userrtl.h:69. */
#define DBCS_CHARSIZE (2)
#define GETAPPVER() GetClientInfo()->dwExpWinVer
#define USERGETCODEPAGE(hdc) GdiGetCodePage(hdc)
#define UserLpkDrawTextEx fpLpkDrawTextEx
#define UserLocalAlloc(flags, bytes) LocalAlloc((flags), (bytes))
#define UserLocalReAlloc(ptr, bytes, flags) LocalReAlloc((HLOCAL)(ptr), (bytes), (flags))
#define UserLocalFree(ptr) LocalFree((HLOCAL)(ptr))
#define UserGetLayout GetLayout
#define UserGetTextCharsetInfo GetTextCharsetInfo
#define UserSetTextAlign SetTextAlign
#define UserGetTextAlign GetTextAlign
#define UserCreateRectRgn CreateRectRgn
#define UserDeleteObject DeleteObject
#define UserGetClipRgn GetClipRgn
#define UserIntersectClipRect IntersectClipRect
#define UserExtSelectClipRgn ExtSelectClipRgn
#define UserAssert(condition) ((void)0)
#define RIPERR1(error, level, text, arg) SetLastError(error)
#define FUNCLOG5(...)
#define FUNCLOG6(...)
#define FUNCLOGVOID6(...)
/* Source-local original wrappers coexist with native USER import consumers. */
#define DrawTextA wow_original_DrawTextA
#define TabbedTextOutA wow_original_TabbedTextOutA
#define GetTabbedTextExtentA wow_original_GetTabbedTextExtentA
#define DrawTextExW wow_original_DrawTextExW
int WINAPI wow_original_DrawTextExW(HDC, LPWSTR, int, LPRECT, UINT, LPDRAWTEXTPARAMS);
#define GdiGetCodePage wow_edit_native_GdiGetCodePage
#define GdiValidateHandle wow_edit_native_GdiValidateHandle
/* XP rtl/draw.c retains its geometry; native USER/GDI own metrics/resources. */
#undef SYSMETRTL
#define SYSMETRTL(index) GetSystemMetrics(SM_##index)
#undef SYSRGBRTL
#define SYSRGBRTL(index) GetSysColor(COLOR_##index)
#undef SYSHBRUSH
#define SYSHBRUSH(index) wow_edit_system_brushes[index]
#define UserSetGraphicsMode SetGraphicsMode
/* XP windows/published/winuser.w:2998; private caption selector mask. */
#define DFCS_CAPTIONALL 0x000F
#endif
