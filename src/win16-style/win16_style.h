#ifndef WIN16_STYLE_H
#define WIN16_STYLE_H
#include <windows.h>

/* Local, same-GUI-thread API. No process/task policy or cross-process ABI.
 * Eligible: captioned top-level windows without menus, scrollbars or custom
 * client edges. S3 deliberately leaves those later integration cases native.
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
