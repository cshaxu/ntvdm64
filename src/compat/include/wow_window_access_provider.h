#ifndef WOW_WINDOW_ACCESS_PROVIDER_H
#define WOW_WINDOW_ACCESS_PROVIDER_H
#include <windows.h>
struct tagWND;

/* Original object operations, borrowed synchronously in the caller's existing
 * window scope. No private layout or additional object registry is exposed. */
typedef struct wow_window_access_provider {
    DWORD (WINAPI *set_long)(struct tagWND *, int, DWORD, BOOL);
    DWORD (WINAPI *get_long)(struct tagWND *, int, BOOL);
    LONG (WINAPI *set_client_long)(HWND, int, LONG, BOOL);
    HMENU (WINAPI *load_menu)(HINSTANCE, LPCWSTR);
    /* Read the existing borrowed object; never resolve a new HWND owner. */
    HWND (WINAPI *window_handle)(struct tagWND *);
    DWORD default_function;
} wow_window_access_provider;

BOOL WINAPI wow_window_access_bind(const wow_window_access_provider *);
const wow_window_access_provider *WINAPI wow_window_access_current(void);
#endif
