#ifndef NTW16S_NATIVE_API_H
#define NTW16S_NATIVE_API_H
#include "api.h"
/* Three native leaves used internally by the renderer and also intercepted by
 * the native consumer. Bind the original USER/UxTheme trampolines once, before
 * creating styled windows. WOW and direct DLL consumers use native defaults.
 * Local same-width ABI; caller retains these code pointers for DLL lifetime. */
typedef struct ntw16s_native_api {
    DWORD size;
    BOOL (WINAPI *get_scroll_info)(HWND,int,LPSCROLLINFO);
    int (WINAPI *set_scroll_info)(HWND,int,LPCSCROLLINFO,BOOL);
    HRESULT (WINAPI *set_window_theme)(HWND,LPCWSTR,LPCWSTR);
} ntw16s_native_api;
NTW16S_API BOOL __cdecl Ntw16sBindNativeApi(const ntw16s_native_api *);
BOOL ntw16s_native_get_scroll_info(HWND,int,LPSCROLLINFO);
int ntw16s_native_set_scroll_info(HWND,int,LPCSCROLLINFO,BOOL);
HRESULT ntw16s_native_set_window_theme(HWND,LPCWSTR,LPCWSTR);
#endif
