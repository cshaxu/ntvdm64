#ifndef WIN16_STYLE_MENU_SCOPE_H
#define WIN16_STYLE_MENU_SCOPE_H
#include "win16_style.h"
typedef struct win16_style_menu_scope win16_style_menu_scope;
/* Same-thread lifetime around one native menu loop. Nested scopes are allowed;
 * owner teardown may end an outer scope before an inner scope finishes.
 * Native popup menus shown/painted on that thread are decorated, including
 * windows created before WM_ENTERMENULOOP. Removed at scope exit.
 * No menu data, input or native tracking is replaced. */
win16_style_menu_scope *win16_style_menu_begin(const win16_style_palette *palette);
BOOL win16_style_menu_end(win16_style_menu_scope *scope);
#endif
