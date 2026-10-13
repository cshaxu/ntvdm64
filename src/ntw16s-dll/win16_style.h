#ifndef WIN16_STYLE_H
#define WIN16_STYLE_H
#include "api.h"

/* Local, same-GUI-thread API. No process/task policy or cross-process ABI.
 * Eligible: top-level and child frames without custom client edges. Frames
 * with initial or later-added scrollbars use the explicit binding below.
 * String menu bars and standard MDI system/restore cells use
 * shared geometry/painting. Popup tracking and MDI management remain native;
 * the component adapts string popup painting and frame/child navigation.
 * Arbitrary owner-drawn/bitmap menu bars are outside this experimental profile.
 * Install/remove preserve the current client origin and dimensions when normal.
 * Caller must remove before unloading this library. Destruction removes itself.
 */
typedef struct win16_style_palette {
    COLORREF active, inactive, active_text, inactive_text;
    COLORREF face, light, shadow, edge;
} win16_style_palette;

NTW16S_API void win16_style_default_palette(win16_style_palette *palette);
NTW16S_API BOOL win16_style_install(HWND window, const win16_style_palette *palette);
NTW16S_API BOOL win16_style_remove(HWND window);
NTW16S_API BOOL win16_style_set_palette(HWND window, const win16_style_palette *palette);
/* Explicit standalone frame-scroll API binding. A subclass alone cannot
 * replace USER's non-message SB_HORZ/SB_VERT query/update entry points.
 * No process hooks are installed here; unstyled windows delegate unchanged. */
NTW16S_API BOOL win16_style_get_scroll_info(HWND window,int bar,LPSCROLLINFO info);
NTW16S_API int win16_style_set_scroll_info(HWND window,int bar,LPCSCROLLINFO info,BOOL redraw);
#endif
