#ifndef WOW_MENU_RESOURCE_BINDINGS_H
#define WOW_MENU_RESOURCE_BINDINGS_H
#include <windows.h>
#include "wow_menu_parser_bindings.h"

/* ADAPTER-WOW-036: resource callbacks selected by original WOW registration.
 * W32 owners retain native/guest discrimination, conversion and lifetime. */
HANDLE APIENTRY W32FindResource(HANDLE, LPCSTR, LPCSTR, WORD);
HANDLE APIENTRY W32LoadResource(HANDLE, HANDLE);
LPSTR APIENTRY W32LockResource(HANDLE, HANDLE);
BOOL APIENTRY W32UnlockResource(HANDLE, HANDLE);
HANDLE WINAPI WOWFindResourceExWCover(HANDLE, LPCWSTR, LPCWSTR, WORD);
int WCSToMBEx(WORD, LPCWSTR, int, LPSTR *, int, BOOL);
HMENU WINAPI wow_user_load_menuA(HINSTANCE, LPCSTR);
HMENU WINAPI wow_user_load_menuW(HINSTANCE, LPCWSTR);

#define FINDRESOURCEEXA(module, name, type, language) \
    W32FindResource(module, type, name, language)
#ifdef WOW_ORIGINAL_MENU_CLIENT
#define LoadMenuA wow_user_load_menuA
#define LoadMenuW wow_user_load_menuW
#define FINDRESOURCEA(module, name, type) W32FindResource(module, type, name, 0)
#define FINDRESOURCEW(module, name, type) WOWFindResourceExWCover(module, (LPCWSTR)(type), name, 0)
#define LOADRESOURCE W32LoadResource
#define LOCKRESOURCE W32LockResource
#define UNLOCKRESOURCE W32UnlockResource
#endif
#endif
