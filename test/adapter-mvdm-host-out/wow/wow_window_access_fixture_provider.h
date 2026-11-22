#ifndef WOW_WINDOW_ACCESS_FIXTURE_PROVIDER_H
#define WOW_WINDOW_ACCESS_FIXTURE_PROVIDER_H
#include "compat/include/wow_window_access_provider.h"
/* Original headers inherit /Gz, while mixed host fixtures use /Gd. Link
 * explicitly typed aliases to the product symbols without changing header
 * inclusion order or other original declarations. */
DWORD WINAPI xxxSetWindowLong(struct tagWND *, int, DWORD, BOOL);
DWORD WINAPI fixture_GetWindowLong(struct tagWND *, int, BOOL);
LONG WINAPI fixture_SetWindowLong(HWND, int, LONG, BOOL);
#pragma comment(linker, "/alternatename:_fixture_GetWindowLong@12=__GetWindowLong@12")
#pragma comment(linker, "/alternatename:_fixture_SetWindowLong@16=__SetWindowLong@16")
HMENU WINAPI wow_user_load_menuW(HINSTANCE, LPCWSTR);
static const wow_window_access_provider wow_fixture_window_access_provider = {
    xxxSetWindowLong, fixture_GetWindowLong, fixture_SetWindowLong, wow_user_load_menuW
};
#endif
