#ifndef WOW_MENU_NATIVE_BINDINGS_H
#define WOW_MENU_NATIVE_BINDINGS_H
#include <windows.h>

HMENU WINAPI wow_menu_native_CreateMenu(void);
HMENU WINAPI wow_menu_native_CreatePopupMenu(void);
HMENU WINAPI wow_menu_native_GetSystemMenu(HWND, BOOL);
BOOL WINAPI wow_menu_native_DestroyMenu(HMENU);
BOOL WINAPI wow_menu_native_AppendMenuA(HMENU, UINT, UINT_PTR, LPCSTR);
BOOL WINAPI wow_menu_native_InsertMenuA(HMENU, UINT, UINT, UINT_PTR, LPCSTR);
BOOL WINAPI wow_menu_native_ModifyMenuA(HMENU, UINT, UINT, UINT_PTR, LPCSTR);
BOOL WINAPI wow_menu_native_ChangeMenuA(HMENU, UINT, LPCSTR, UINT, UINT);
BOOL WINAPI wow_menu_native_InsertMenuItemW(HMENU, UINT, BOOL, LPCMENUITEMINFOW);
BOOL WINAPI wow_menu_native_DeleteMenu(HMENU, UINT, UINT);
BOOL WINAPI wow_menu_native_RemoveMenu(HMENU, UINT, UINT);
DWORD WINAPI wow_menu_native_CheckMenuItem(HMENU, UINT, UINT);
BOOL WINAPI wow_menu_native_EnableMenuItem(HMENU, UINT, UINT);
BOOL WINAPI wow_menu_native_HiliteMenuItem(HWND, HMENU, UINT, UINT);
BOOL WINAPI wow_menu_native_SetMenuItemBitmaps(HMENU, UINT, UINT, HBITMAP, HBITMAP);
BOOL WINAPI wow_menu_native_SetMenu(HWND, HMENU);
BOOL WINAPI wow_menu_native_refresh_window(HWND);
BOOL WINAPI wow_menu_native_refresh_menu(HMENU);

/* Selected only at original WOW/menu-client compile boundaries. The owned
 * implementation includes this header without redirects and calls USER32. */
#if defined(WOW_USER_PRIVATE_REDIRECT) || defined(WOW_ORIGINAL_MENU_CLIENT)
#define CreateMenu wow_menu_native_CreateMenu
#define CreatePopupMenu wow_menu_native_CreatePopupMenu
#define GetSystemMenu wow_menu_native_GetSystemMenu
#define DestroyMenu wow_menu_native_DestroyMenu
#define AppendMenuA wow_menu_native_AppendMenuA
#define InsertMenuA wow_menu_native_InsertMenuA
#define ModifyMenuA wow_menu_native_ModifyMenuA
#define ChangeMenuA wow_menu_native_ChangeMenuA
#define InsertMenuItemW wow_menu_native_InsertMenuItemW
#define DeleteMenu wow_menu_native_DeleteMenu
#define RemoveMenu wow_menu_native_RemoveMenu
#define CheckMenuItem wow_menu_native_CheckMenuItem
#define EnableMenuItem wow_menu_native_EnableMenuItem
#define HiliteMenuItem wow_menu_native_HiliteMenuItem
#define SetMenuItemBitmaps wow_menu_native_SetMenuItemBitmaps
#define SetMenu wow_menu_native_SetMenu
#endif
#endif
