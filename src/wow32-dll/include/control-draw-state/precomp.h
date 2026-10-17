#ifndef WOW_CONTROL_DRAW_STATE_PRECOMP_H
#define WOW_CONTROL_DRAW_STATE_PRECOMP_H
#include "wow_control_draw_state.h"
#include "wow_edit_drawing_bindings.h"
#include "wow_edit_native_bindings.h"
#include <wchar.h>
#include "opennt-abi/host-compat/include/wow_control_resource_abi.h"
#define ghdcGray (wow_control_draw_current->gray_dc)
#define ghdcBits2 (wow_control_draw_current->bits_dc)
#define gcxGray (wow_control_draw_current->gray_width)
#define gcyGray (wow_control_draw_current->gray_height)
#define gcsHdc (wow_control_draw_current->lock)
#define oemInfo (wow_control_draw_current->atlas)
#define RtlEnterCriticalSection EnterCriticalSection
#define RtlLeaveCriticalSection LeaveCriticalSection
#define ghFontSys ((HFONT)GetStockObject(SYSTEM_FONT))
#define ghbrGray wow_edit_gray_brush
#define SYSMET(index) GetSystemMetrics(SM_##index)
#define SYSHBR(index) wow_edit_system_brushes[COLOR_##index]
#define UserAssert(condition) ((void)0)
#define RIPMSG0(level, text) ((void)0)
#define PolyPatBlt wow_edit_native_PolyPatBlt
#define NtUserGetIconSize wow_control_icon_size
#define NtUserBitBltSysBmp wow_control_atlas_blit
#define SelectObject wow_control_draw_select
BOOL WINAPI BitBltSysBmp(HDC, int, int, UINT);
VOID WINAPI BltColor(HDC, HBRUSH, HDC, int, int, int, int, int, int, BOOL);
BOOL WINAPI PSMGetTextExtent(HDC, LPWSTR, int, PSIZE);
VOID WINAPI PSMTextOut(HDC, int, int, LPWSTR, int);
#endif
