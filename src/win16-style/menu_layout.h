#ifndef WIN16_STYLE_MENU_LAYOUT_H
#define WIN16_STYLE_MENU_LAYOUT_H
#include <windows.h>

/* Window-relative bar geometry shared by painting, hit testing and popup
 * placement. Sizes come from the selected renderer. No HMENU is retained.
 * Only MFT_MENUBREAK and MFT_RIGHTJUSTIFY affect arrangement. All items from
 * the first right-justified item form a group aligned within each row; oversized items are clipped
 * to the available width. Results are undefined on failure. */
BOOL win16_style_menu_arrange(POINT origin,int width,const SIZE *sizes,
    const UINT *flags,UINT count,RECT *items,int *height);
int win16_style_menu_hit(const RECT *items,UINT count,POINT point);
#endif
