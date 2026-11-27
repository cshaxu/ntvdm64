#ifndef WOW_DIALOG_FINALIZATION_PROVIDER_H
#define WOW_DIALOG_FINALIZATION_PROVIDER_H
#include <windows.h>
struct tagWND;
struct _DLG;
typedef struct wow_dialog_finalization_provider {
    LONG (WINAPI *worker)(struct tagWND *, UINT, WPARAM, LPARAM, DWORD);
    BOOL (WINAPI *end)(HWND, INT_PTR);
    BOOL (WINAPI *map)(HWND, LPRECT);
    int (WINAPI *list)(HWND, LPSTR, int, int, UINT);
    int (WINAPI *combo_list)(HWND, LPSTR, int, int, UINT);
    BOOL (WINAPI *select)(HWND, LPSTR, int, int);
    BOOL (WINAPI *combo_select)(HWND, LPSTR, int, int);
    BOOL (WINAPI *owns)(struct tagWND *);
    BOOL (WINAPI *finalize)(struct tagWND *,
        LONG (WINAPI *)(struct tagWND *, UINT, WPARAM, LPARAM, DWORD));
    BOOL (WINAPI *set_pointer)(struct tagWND *, struct _DLG *);
} wow_dialog_finalization_provider;
BOOL WINAPI wow_dialog_finalization_bind(const wow_dialog_finalization_provider *);
const wow_dialog_finalization_provider *WINAPI wow_dialog_finalization_current(void);
#endif
