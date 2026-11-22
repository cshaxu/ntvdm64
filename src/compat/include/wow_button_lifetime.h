#ifndef WOW_BUTTON_LIFETIME_H
#define WOW_BUTTON_LIFETIME_H
#include <windows.h>
#include <winternl.h>
#include "wow_button_resources.h"
struct tagWND;
typedef struct _LOOKASIDE *PLOOKASIDE;
typedef PVOID (WINAPI *wow_button_data_provider)(struct tagWND *);
typedef VOID (WINAPI *wow_button_release_provider)(PLOOKASIDE, PVOID);
BOOL WINAPI wow_button_bind(wow_button_data_provider, wow_button_release_provider);
/* Caller-owned stack scope. The enclosing USER borrow retains the exact WND;
 * this scope delays only private BUTN reuse across original client callouts. */
typedef struct wow_button_scope {
    struct wow_button_scope *previous;
    struct tagWND *window;
    PLOOKASIDE release_pool;
    PVOID release_value;
    wow_button_resources resources;
    BOOL resources_initialized;
} wow_button_scope;
VOID WINAPI wow_button_scope_enter(wow_button_scope *, struct tagWND *);
VOID WINAPI wow_button_scope_leave(wow_button_scope *);
/* Existing control entry owns task/borrow scopes and the retained WND. */
LRESULT WINAPI wow_button_invoke(HWND, UINT, WPARAM, LPARAM, WNDPROC);
VOID WINAPI wow_button_free_state(PLOOKASIDE, PVOID);
wow_button_resources *WINAPI wow_button_current_resources(void);
BOOL WINAPI wow_button_atlas_blit(HDC, int, int, int, int, int, int, DWORD);
BOOL WINAPI wow_button_BitBltSysBmp(HDC, int, int, UINT);
#endif
