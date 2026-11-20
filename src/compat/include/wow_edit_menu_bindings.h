#ifndef WOW_EDIT_MENU_BINDINGS_H
#define WOW_EDIT_MENU_BINDINGS_H
#include "wow_menu_resource_bindings.h"
#include "wow_menu_native_bindings.h"
#include "wow_user_private_access.h"

/* OPENNT-HOST-087 resources are linked into this already loaded provider,
 * not modern USER32. Original WOW32 DllInitialize owns the module handle. */
extern HANDLE hmodWOW32;

#ifdef WOW_ORIGINAL_EDIT_CLIENT
#define hmodUser ((HINSTANCE)hmodWOW32)
#undef LoadMenu
#define LoadMenu wow_user_load_menuW
#define NtUserTrackPopupMenuEx wow_menu_native_TrackPopupMenuEx
#define NtUserDestroyMenu wow_menu_native_DestroyMenu
#define NtUserGetSystemMenu wow_menu_native_GetSystemMenu
#define EnableMenuItem wow_menu_native_EnableMenuItem
#define NtUserSetFocus wow_native_SetFocus
#define NtUserIsClipboardFormatAvailable IsClipboardFormatAvailable
#undef SendMessage
#define SendMessage wow_native_SendMessageW
#endif
#endif
