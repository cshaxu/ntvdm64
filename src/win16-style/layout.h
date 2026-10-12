#ifndef WIN16_STYLE_LAYOUT_H
#define WIN16_STYLE_LAYOUT_H
#include <windows.h>

/* Shared geometry for paint and hit-testing; all rectangles window-relative. */
typedef struct win16_style_layout {
    RECT outer, client, caption, system, minimize, maximize, title;
    int border, unit;
    SIZE corner;
    BOOL sizeable, dialog_frame;
} win16_style_layout;
BOOL win16_style_measure(int width, int height, UINT dpi, DWORD style, DWORD ex_style,
    BOOL maximized, win16_style_layout *layout);
LRESULT win16_style_hit(const win16_style_layout *layout, POINT point);
#endif
