#ifndef WIN16_STYLE_H
#define WIN16_STYLE_H
#include <windows.h>

/* Local, same-GUI-thread API. No process/task policy or cross-process ABI.
 * Eligible: top-level and child frames without scrollbars or custom
 * client edges. String menu bars and standard MDI system/restore cells use
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

void win16_style_default_palette(win16_style_palette *palette);
BOOL win16_style_install(HWND window, const win16_style_palette *palette);
BOOL win16_style_remove(HWND window);
BOOL win16_style_set_palette(HWND window, const win16_style_palette *palette);
#endif
