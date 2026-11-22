#ifndef WOW_DIALOG_FINALIZATION_PROVIDER_H
#define WOW_DIALOG_FINALIZATION_PROVIDER_H
#include <windows.h>
struct tagWND;
typedef struct wow_dialog_finalization_provider {
    LONG (WINAPI *worker)(struct tagWND *, UINT, WPARAM, LPARAM, DWORD);
    BOOL (WINAPI *end)(HWND, int);
    BOOL (WINAPI *map)(HWND, LPRECT);
    int (WINAPI *list)(HWND, LPSTR, int, int, UINT);
    int (WINAPI *combo_list)(HWND, LPSTR, int, int, UINT);
    BOOL (WINAPI *select)(HWND, LPSTR, int, int);
    BOOL (WINAPI *combo_select)(HWND, LPSTR, int, int);
} wow_dialog_finalization_provider;
BOOL WINAPI wow_dialog_finalization_bind(const wow_dialog_finalization_provider *);
#endif
