#ifndef WIN16_STYLE_MENU_ITEMS_H
#define WIN16_STYLE_MENU_ITEMS_H
#include "menu_paint.h"
typedef struct win16_style_menu_item win16_style_menu_item;
BOOL win16_style_menu_prepare(HMENU menu,win16_style_menu_item **items,unsigned depth);
void win16_style_menu_restore_depth(unsigned depth,win16_style_menu_item **items);
void win16_style_menu_restore(win16_style_menu_item **items);
void win16_style_menu_restore_popup(HMENU menu,win16_style_menu_item **items);
BOOL win16_style_menu_draw(win16_style_menu_item *items,const DRAWITEMSTRUCT *draw,
    HFONT font,UINT dpi,const win16_style_palette *palette);
BOOL win16_style_menu_size(win16_style_menu_item *items,MEASUREITEMSTRUCT *measure,HFONT font,UINT dpi);
BOOL win16_style_menu_char(win16_style_menu_item *items,HMENU menu,WCHAR key,LRESULT *result);
#endif
