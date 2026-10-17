#ifndef WOW_BUTTON_LIFETIME_H
#define WOW_BUTTON_LIFETIME_H
#include "wow_edit_lookaside_bindings.h"
#include "wow_button_resources.h"
struct tagWND;
struct tagBUTNWND;
/* Caller-owned stack scope. The enclosing USER borrow retains the exact WND;
 * this scope delays only private BUTN reuse across original client callouts. */
typedef struct wow_button_scope {
    struct wow_button_scope *previous;
    struct tagBUTNWND *window;
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
