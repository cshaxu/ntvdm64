#ifndef WIN16_STYLE_MENU_BAR_H
#define WIN16_STYLE_MENU_BAR_H
#include "menu_paint.h"
#include "menu_layout.h"
typedef struct win16_style_bar_item {
    WCHAR *text;
    UINT state,id;
    HMENU popup;
    HBITMAP control;
} win16_style_bar_item;
typedef struct win16_style_bar {
    UINT count;
    int height;
    RECT *rects;
    win16_style_bar_item *items;
} win16_style_bar;
/* Transient copied text/geometry; HMENU handles are borrowed. String-only
 * top-level entries are supported, not application owner-drawn/bitmap bars. */
BOOL win16_style_bar_read(HMENU menu,HFONT font,UINT dpi,POINT origin,int width,win16_style_bar *bar);
void win16_style_bar_free(win16_style_bar *bar);
void win16_style_bar_paint(const win16_style_bar *bar,HDC dc,HFONT font,UINT dpi,const win16_style_palette *palette);
/* Internal shared original-resource painter for MDI's system/restore cells. */
void win16_style_merged_control(HDC dc,RECT bounds,BOOL system,BOOL selected,int unit,const win16_style_palette *palette);
#endif
