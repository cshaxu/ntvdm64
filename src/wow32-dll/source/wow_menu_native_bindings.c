#include "wow_menu_native_bindings.h"
#include "wow_menu_client_bindings.h"
#include "wow_user_runtime.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

static wow_user_runtime_thread *menu_context(void)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    if (!binding || !binding->thread || !binding->thread->ptdb ||
            !binding->thread->ppi || !binding->thread->ppi->objects) {
        SetLastError(ERROR_INVALID_STATE); return NULL;
    }
    return binding;
}

static BOOL menu_refresh(wow_user_runtime_thread *binding, HMENU menu)
{
    BOOL entered = !binding->exclusive_held, result = FALSE;
    DWORD error;
    if (entered && !wow_user_runtime_enter(binding)) return FALSE;
    __try {
        result = wow_menu_client_retire_destroyed(binding->thread->ppi);
        if (result && menu)
            result = wow_menu_client_publish_tree(binding->thread->ppi,
                binding->thread->ptdb->hTaskWow, menu);
    } __finally {
        error = GetLastError();
        if (entered) (void)wow_user_runtime_leave(binding);
        SetLastError(error);
    }
    return result;
}

BOOL WINAPI wow_menu_native_refresh_menu(HMENU menu)
{
    wow_user_runtime_thread *binding = menu_context();
    return binding && menu_refresh(binding, menu);
}

static HMENU menu_resolve(wow_user_runtime_thread *binding, HMENU menu)
{
    BOOL entered = !binding->exclusive_held;
    HMENU result;
    if (entered && !wow_user_runtime_enter(binding)) return NULL;
    result = wow_menu_client_full_handle(binding->thread->ppi->objects, menu);
    if (entered) (void)wow_user_runtime_leave(binding);
    if (!result) SetLastError(ERROR_INVALID_MENU_HANDLE);
    return result;
}

static HMENU menu_create(BOOL popup)
{
    wow_user_runtime_thread *binding = menu_context();
    HMENU menu;
    DWORD error;
    if (!binding) return NULL;
    menu = popup ? CreatePopupMenu() : CreateMenu();
    if (!menu) return NULL;
    error = GetLastError();
    if (!menu_refresh(binding, menu)) {
        DWORD failure = GetLastError();
        DestroyMenu(menu);
        (void)menu_refresh(binding, NULL);
        SetLastError(failure);
        return NULL;
    }
    SetLastError(error);
    return menu;
}

HMENU WINAPI wow_menu_native_CreateMenu(void) { return menu_create(FALSE); }
HMENU WINAPI wow_menu_native_CreatePopupMenu(void) { return menu_create(TRUE); }

HMENU WINAPI wow_menu_native_GetSystemMenu(HWND window, BOOL revert)
{
    wow_user_runtime_thread *binding = menu_context();
    HMENU result;
    DWORD error;
    if (!binding) return NULL;
    result = GetSystemMenu(window, revert);
    error = GetLastError();
    if (!menu_refresh(binding, result)) return NULL;
    SetLastError(error);
    return result;
}

/* Retain each native operation and its result convention. The extra step is
 * publication of its completed query state, not reimplementation of mutation
 * or item selection. In particular, original xxxHiliteMenuItem can draw an
 * owner-drawn item. Release an inherited USER data lock around native calls;
 * retaining WOW execution ownership is independent of that data lock.
 * Publication itself invokes no guest callback. */
#define MENU_MUTATION(type, name, args, call, failed, view) \
type WINAPI wow_menu_native_##name args \
{ \
    wow_user_runtime_thread *binding = menu_context(); \
    type result = failed; DWORD error; BOOL held, restored = TRUE; \
    if (!binding || !(menu = menu_resolve(binding, menu))) return failed; \
    held = binding->exclusive_held; \
    if (held && !wow_user_runtime_leave(binding)) return failed; \
    __try { result = name call; } \
    __finally { \
        error = GetLastError(); \
        if (held) restored = wow_user_runtime_enter(binding); \
        if (restored) SetLastError(error); \
    } \
    if (!restored) return failed; \
    if (result != failed && !menu_refresh(binding, view)) return failed; \
    SetLastError(error); return result; \
}
MENU_MUTATION(BOOL, DestroyMenu, (HMENU menu), (menu), FALSE, NULL)
MENU_MUTATION(BOOL, AppendMenuA, (HMENU menu, UINT flags, UINT_PTR id, LPCSTR text),
    (menu, flags, id, text), FALSE, menu)
MENU_MUTATION(BOOL, InsertMenuA, (HMENU menu, UINT item, UINT flags, UINT_PTR id, LPCSTR text),
    (menu, item, flags, id, text), FALSE, menu)
MENU_MUTATION(BOOL, ModifyMenuA, (HMENU menu, UINT item, UINT flags, UINT_PTR id, LPCSTR text),
    (menu, item, flags, id, text), FALSE, menu)
MENU_MUTATION(BOOL, ChangeMenuA, (HMENU menu, UINT item, LPCSTR text, UINT id, UINT flags),
    (menu, item, text, id, flags), FALSE, menu)
MENU_MUTATION(BOOL, InsertMenuItemW, (HMENU menu, UINT item, BOOL position, LPCMENUITEMINFOW info),
    (menu, item, position, info), FALSE, menu)
MENU_MUTATION(BOOL, DeleteMenu, (HMENU menu, UINT item, UINT flags),
    (menu, item, flags), FALSE, menu)
MENU_MUTATION(BOOL, RemoveMenu, (HMENU menu, UINT item, UINT flags),
    (menu, item, flags), FALSE, menu)
MENU_MUTATION(DWORD, CheckMenuItem, (HMENU menu, UINT item, UINT flags),
    (menu, item, flags), (DWORD)-1, menu)
MENU_MUTATION(BOOL, EnableMenuItem, (HMENU menu, UINT item, UINT flags),
    (menu, item, flags), (BOOL)-1, menu)
MENU_MUTATION(BOOL, HiliteMenuItem, (HWND window, HMENU menu, UINT item, UINT flags),
    (window, menu, item, flags), FALSE, menu)
MENU_MUTATION(BOOL, SetMenuItemBitmaps, (HMENU menu, UINT item, UINT flags, HBITMAP off, HBITMAP on),
    (menu, item, flags, off, on), FALSE, menu)
#undef MENU_MUTATION

BOOL WINAPI wow_menu_native_refresh_window(HWND window)
{
    wow_user_runtime_thread *binding = menu_context();
    HWND full;
    HMENU menu;
    WINDOWINFO info = { sizeof(info) };
    BOOL entered, result = FALSE;
    DWORD error;
    if (!binding) return FALSE;
    entered = !binding->exclusive_held;
    if (entered && !wow_user_runtime_enter(binding)) return FALSE;
    __try {
        full = wow_user_window_full_handle(binding->thread->ppi->objects, LOWORD(window));
        if (!full || !GetWindowInfo(window, &info)) __leave;
        if (info.dwStyle & WS_CHILD) { result = TRUE; __leave; }
        menu = GetMenu(window);
        if (!menu_refresh(binding, menu)) __leave;
        result = mvdm_softpc_wow_page_domain_window_menu(
            (ULONG)(ULONG_PTR)full, (ULONG)(ULONG_PTR)menu);
    } __finally {
        error = GetLastError();
        if (entered) (void)wow_user_runtime_leave(binding);
        SetLastError(error);
    }
    return result;
}

BOOL WINAPI wow_menu_native_SetMenu(HWND window, HMENU menu)
{
    wow_user_runtime_thread *binding = menu_context();
    BOOL result = FALSE, held, restored = TRUE;
    DWORD error;
    if (!binding || (menu && !(menu = menu_resolve(binding, menu)))) return FALSE;
    /* Original mnapi.c::xxxSetMenu associates the menu before
     * xxxRedrawFrame. Native USER may likewise call the window while
     * recalculating its frame. Follow the existing USER callout contract:
     * release only an inherited data lock, never WOW task ownership, and
     * restore it even when the native call unwinds. */
    held = binding->exclusive_held;
    if (held && !wow_user_runtime_leave(binding)) return FALSE;
    __try {
        result = SetMenu(window, menu);
    } __finally {
        error = GetLastError();
        if (held) restored = wow_user_runtime_enter(binding);
        if (restored) SetLastError(error);
    }
    if (!restored) return FALSE;
    if (result && !wow_menu_native_refresh_window(window)) return FALSE;
    SetLastError(error);
    return result;
}
