#include "wow_menu_native_bindings.h"
#include "wow_menu_client_bindings.h"
#include "wow_user_runtime.h"
#include "wow_user_task_lifecycle.h"
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

static BOOL menu_publish(wow_user_runtime_thread *binding, HMENU menu, BOOL created,
    BOOL local_state)
{
    BOOL entered = !binding->exclusive_held, result = FALSE;
    DWORD error;
    if (entered && !wow_user_runtime_enter(binding)) return FALSE;
    __try {
        result = TRUE;
        /* A completed mutation must replace its parent ITEM edges before
         * invalid native submenus can release their copied backing. */
        if (created)
            result = wow_menu_client_retire_destroyed(binding->thread->ppi);
        if (result && menu)
            result = created ? wow_menu_client_register_created(binding->thread->ppi,
                binding->thread->ptdb->hTaskWow, menu) :
                (local_state ? wow_menu_client_publish_local_tree(binding->thread->ppi, menu) :
                    wow_menu_client_publish_tree(binding->thread->ppi, menu));
        if (result && !created)
            result = wow_menu_client_retire_destroyed(binding->thread->ppi);
    } __finally {
        error = GetLastError();
        if (entered) (void)wow_user_runtime_leave(binding);
        SetLastError(error);
    }
    return result;
}

static BOOL menu_refresh(wow_user_runtime_thread *binding, HMENU menu)
{
    return menu_publish(binding, menu, FALSE, FALSE);
}

BOOL WINAPI wow_menu_native_refresh_menu(HMENU menu)
{
    wow_user_runtime_thread *binding = menu_context();
    return binding && menu_refresh(binding, menu);
}

BOOL WINAPI wow_menu_native_refresh_message(UINT message, WPARAM wp, LPARAM lp)
{
    HMENU menu;
    DWORD error = GetLastError();
    /* Original wmdisp32.c converts these native menu identities to short
     * guest handles. The menu need not be GetMenu(callback_window): popup
     * tracking and system menus have their own roots. Observe, never claim
     * allocation ownership. Preserve synthetic/terminal messages as well;
     * validating their contents is not a new dispatch policy. */
    switch (message) {
    case WM_INITMENU:
    case WM_INITMENUPOPUP:
        menu = (HMENU)wp;
        break;
    case WM_MENUSELECT:
    case WM_MENUCHAR:
        menu = (HMENU)lp;
        break;
    case WM_DRAWITEM:
        if (!lp || ((const DRAWITEMSTRUCT *)lp)->CtlType != ODT_MENU) return TRUE;
        menu = (HMENU)((const DRAWITEMSTRUCT *)lp)->hwndItem;
        break;
    case WM_DELETEITEM:
        if (!lp || ((const DELETEITEMSTRUCT *)lp)->CtlType != ODT_MENU) return TRUE;
        menu = (HMENU)((const DELETEITEMSTRUCT *)lp)->hwndItem;
        break;
    case WM_COMPAREITEM:
        if (!lp || ((const COMPAREITEMSTRUCT *)lp)->CtlType != ODT_MENU) return TRUE;
        menu = (HMENU)((const COMPAREITEMSTRUCT *)lp)->hwndItem;
        break;
    case WM_NEXTMENU:
        if (!lp) return TRUE;
        menu = ((const MDINEXTMENU *)lp)->hmenuIn;
        break;
    default:
        return TRUE;
    }
    if (menu && IsMenu(menu) && !wow_menu_native_refresh_menu(menu))
        return FALSE;
    SetLastError(error);
    return TRUE;
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
    if (!menu_publish(binding, menu, TRUE, TRUE)) {
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
    wow_user_native_call call;
    HMENU result = NULL;
    BOOL restored;
    DWORD error;
    if (!binding) return NULL;
    if (!wow_user_native_call_begin(&call, window)) return NULL;
    __try { result = GetSystemMenu(window, revert); }
    __finally {
        error = GetLastError();
        restored = wow_user_native_call_end(&call);
        if (restored) SetLastError(error);
    }
    if (!restored) return NULL;
    {
        DWORD owner = 0;
        GetWindowThreadProcessId(window, &owner);
        if (!menu_publish(binding, result, FALSE, owner == GetCurrentProcessId()))
            wow_user_native_call_failed("publish-system-menu", GetLastError());
    }
    SetLastError(error);
    return result;
}

BOOL WINAPI wow_menu_native_TrackPopupMenu(HMENU menu, UINT flags,
    int x, int y, int reserved, HWND window, const RECT *exclude)
{
    wow_user_runtime_thread *binding = menu_context();
    wow_user_native_call call;
    BOOL result = FALSE, restored;
    DWORD error;
    if (!binding || !(menu = menu_resolve(binding, menu)) ||
            !menu_refresh(binding, menu)) return FALSE;
    if (!wow_user_native_call_begin(&call, window)) return FALSE;
    __try { result = TrackPopupMenu(menu, flags, x, y, reserved, window, exclude); }
    __finally {
        error = GetLastError();
        restored = wow_user_native_call_end(&call);
        if (restored) SetLastError(error);
    }
    if (!restored) return FALSE;
    /* The common return boundary publishes all callback mutations, even on
     * cancellation or destruction of the tracked menu/owner window. */
    SetLastError(error);
    return result;
}

BOOL WINAPI wow_menu_native_DrawMenuBar(HWND window)
{
    wow_user_runtime_thread *binding = menu_context();
    wow_user_native_call call;
    BOOL result = FALSE, restored;
    DWORD error;
    if (!binding) return FALSE;
    if (!wow_user_native_call_begin(&call, window)) return FALSE;
    __try { result = DrawMenuBar(window); }
    __finally {
        error = GetLastError();
        restored = wow_user_native_call_end(&call);
        if (restored) SetLastError(error);
    }
    if (!restored) return FALSE;
    SetLastError(error);
    return result;
}

/* Retain each native operation and its result convention. The extra step is
 * publication of its completed query state, not reimplementation of mutation
 * or item selection. In particular, original xxxHiliteMenuItem can draw an
 * owner-drawn item. Release an inherited USER data lock around native calls;
 * retaining WOW execution ownership is independent of that data lock.
 * Publication itself invokes no guest callback. */
#define MENU_MUTATION(type, name, args, native_args, failed, target) \
type WINAPI wow_menu_native_##name args \
{ \
    wow_user_runtime_thread *binding = menu_context(); \
    wow_user_native_call call; \
    type result = failed; DWORD error; BOOL restored; \
    if (!binding || !(menu = menu_resolve(binding, menu))) return failed; \
    if (!wow_user_native_call_begin(&call, target)) return failed; \
    __try { result = name native_args; } \
    __finally { \
        error = GetLastError(); \
        restored = wow_user_native_call_end(&call); \
        if (restored) SetLastError(error); \
    } \
    if (!restored) return failed; \
    if (IsMenu(menu) && !menu_refresh(binding, menu)) \
        wow_user_native_call_failed("publish-menu-mutation", GetLastError()); \
    SetLastError(error); return result; \
}
MENU_MUTATION(BOOL, DestroyMenu, (HMENU menu), (menu), FALSE, NULL)
MENU_MUTATION(BOOL, AppendMenuA, (HMENU menu, UINT flags, UINT_PTR id, LPCSTR text),
    (menu, flags, id, text), FALSE, NULL)
MENU_MUTATION(BOOL, InsertMenuA, (HMENU menu, UINT item, UINT flags, UINT_PTR id, LPCSTR text),
    (menu, item, flags, id, text), FALSE, NULL)
MENU_MUTATION(BOOL, ModifyMenuA, (HMENU menu, UINT item, UINT flags, UINT_PTR id, LPCSTR text),
    (menu, item, flags, id, text), FALSE, NULL)
MENU_MUTATION(BOOL, ChangeMenuA, (HMENU menu, UINT item, LPCSTR text, UINT id, UINT flags),
    (menu, item, text, id, flags), FALSE, NULL)
MENU_MUTATION(BOOL, InsertMenuItemW, (HMENU menu, UINT item, BOOL position, LPCMENUITEMINFOW info),
    (menu, item, position, info), FALSE, NULL)
MENU_MUTATION(BOOL, DeleteMenu, (HMENU menu, UINT item, UINT flags),
    (menu, item, flags), FALSE, NULL)
MENU_MUTATION(BOOL, RemoveMenu, (HMENU menu, UINT item, UINT flags),
    (menu, item, flags), FALSE, NULL)
MENU_MUTATION(DWORD, CheckMenuItem, (HMENU menu, UINT item, UINT flags),
    (menu, item, flags), (DWORD)-1, NULL)
MENU_MUTATION(BOOL, EnableMenuItem, (HMENU menu, UINT item, UINT flags),
    (menu, item, flags), (BOOL)-1, NULL)
MENU_MUTATION(BOOL, HiliteMenuItem, (HWND window, HMENU menu, UINT item, UINT flags),
    (window, menu, item, flags), FALSE, window)
MENU_MUTATION(BOOL, SetMenuItemBitmaps, (HMENU menu, UINT item, UINT flags, HBITMAP off, HBITMAP on),
    (menu, item, flags, off, on), FALSE, NULL)
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
        /* Commit the WND edge before collecting detached menus. Calling
         * menu_refresh here would collect while the old WND still refers
         * to the old menu. */
        if (menu && !wow_menu_client_publish_local_tree(binding->thread->ppi, menu)) __leave;
        result = mvdm_softpc_wow_page_domain_window_menu(
            (ULONG)(ULONG_PTR)full, (ULONG)(ULONG_PTR)menu);
        if (result) result = wow_menu_client_retire_destroyed(binding->thread->ppi);
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
    wow_user_native_call call;
    BOOL result = FALSE, restored;
    DWORD error;
    if (!binding || (menu && !(menu = menu_resolve(binding, menu)))) return FALSE;
    /* Original mnapi.c::xxxSetMenu associates the menu before
     * xxxRedrawFrame. Native USER may likewise call the window while
     * recalculating its frame. Follow the existing USER callout contract:
     * release only an inherited data lock, never WOW task ownership, and
     * restore it even when the native call unwinds. */
    if (!wow_user_native_call_begin(&call, window)) return FALSE;
    __try {
        result = SetMenu(window, menu);
    } __finally {
        error = GetLastError();
        restored = wow_user_native_call_end(&call);
        if (restored) SetLastError(error);
    }
    if (!restored) return FALSE;
    SetLastError(error);
    return result;
}
