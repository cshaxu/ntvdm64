#ifndef WOW_DEFAULT_WINDOW_PROVIDER_H
#define WOW_DEFAULT_WINDOW_PROVIDER_H
#include <windows.h>
struct tagWND;
typedef LONG (WINAPI *wow_default_window_provider)(struct tagWND *, UINT, WPARAM, LPARAM, DWORD);
BOOL WINAPI wow_default_window_bind(wow_default_window_provider);
#endif
