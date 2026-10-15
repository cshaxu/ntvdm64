#include "wow_menu_client_bindings.h"
#include "wow_cleanup_bindings.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

/* Original cleanup.c reads hTaskWow through the adapted PROCOBJHEAD first
 * member. The original process-owned scan remains the destruction policy;
 * this record is not a second USER menu or a native-layout overlay. */
typedef struct wow_menu_client_object {
    DWORD hTaskWow;
    HMENU native;
    BOOL visiting;
} wow_menu_client_object;

HMENU WINAPI wow_menu_client_full_handle(wow_user_object_table *table, HMENU menu)
{
    ULONG value = (ULONG)(ULONG_PTR)menu;
    WORD index = LOWORD(value);
    wow_cleanup_handle *entry;
    wow_menu_client_object *object;
    if (!table || !index || index == 1u || index == 0xffffu) return NULL;
    entry = &table->entries[index];
    if (entry->bType != TYPE_MENU || !(object = entry->phead)) return NULL;
    if (value != (ULONG)(ULONG_PTR)object->native &&
            value != (ULONG)(LONG)(SHORT)index && value != (ULONG)index)
        return NULL;
    return object->native;
}

static BOOL publish_tree(wow_task_order_process *process,
    DWORD task, HMENU menu, BOOL registering)
{
    wow_user_object_table *table;
    wow_cleanup_handle *entry;
    wow_menu_client_object *object;
    mvdm_softpc_wow_menu_query_item *items = NULL;
    ULONG value = (ULONG)(ULONG_PTR)menu;
    WORD index = LOWORD(value);
    int count, i;
    BOOL created = FALSE, result = FALSE;
    if (!process || !(table = process->objects) || !index || index == 1u ||
            index == 0xffffu || !IsMenu(menu)) {
        SetLastError(ERROR_INVALID_MENU_HANDLE); return FALSE;
    }
    entry = &table->entries[index];
    if (entry->bType == TYPE_FREE) {
        /* New producers supply the full identity returned by USER. A native
         * uniqueness value of zero is not itself invalid; guest short aliases
         * must be resolved by the enrolled-handle path before reaching here. */
        object = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*object));
        if (!object) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
        object->hTaskWow = registering ? task : 0;
        object->native = menu;
        entry->phead = object;
        entry->pOwner = registering ? process : NULL;
        entry->wUniq = HIWORD(value);
        entry->bType = TYPE_MENU;
        created = TRUE;
        if (index > table->last_handle) table->last_handle = index;
    } else {
        menu = wow_menu_client_full_handle(table, menu);
        if (!menu || (entry->pOwner && entry->pOwner != process)) {
            SetLastError(ERROR_INVALID_MENU_HANDLE); return FALSE;
        }
        if (registering) {
            SetLastError(ERROR_ALREADY_EXISTS); return FALSE;
        }
        object = entry->phead;
    }
    if (object->visiting) {
        SetLastError(ERROR_INVALID_DATA); return FALSE;
    }
    object->visiting = TRUE;
    __try {
        count = GetMenuItemCount(menu);
        if (count < 0) __leave;
        if ((SIZE_T)count > (SIZE_T)-1 / sizeof(*items)) {
            SetLastError(ERROR_ARITHMETIC_OVERFLOW); __leave;
        }
        if (count) {
            items = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                (SIZE_T)count * sizeof(*items));
            if (!items) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); __leave; }
        }
        for (i = 0; i < count; ++i) {
            MENUITEMINFOW info = { sizeof(info) };
            info.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
            if (!GetMenuItemInfoW(menu, (UINT)i, TRUE, &info)) __leave;
            if (info.hSubMenu &&
                    !publish_tree(process, 0, info.hSubMenu, FALSE))
                __leave;
            items[i].type = info.fType;
            items[i].state = info.fState;
            items[i].id = info.wID;
            items[i].submenu = (ULONG)(ULONG_PTR)info.hSubMenu;
        }
        if (!mvdm_softpc_wow_page_domain_publish_menu(
                (ULONG)(ULONG_PTR)object->native, object->hTaskWow,
                (ULONG)count, items)) {
            SetLastError(ERROR_INVALID_STATE); __leave;
        }
        result = TRUE;
    } __finally {
        DWORD error = GetLastError();
        object->visiting = FALSE;
        if (items) HeapFree(GetProcessHeap(), 0, items);
        if (!result && created) {
            ZeroMemory(entry, sizeof(*entry));
            HeapFree(GetProcessHeap(), 0, object);
        }
        SetLastError(error);
    }
    return result;
}

BOOL WINAPI wow_menu_client_register_created(wow_task_order_process *process,
    DWORD task, HMENU menu)
{
    if (!task) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    return publish_tree(process, task, menu, TRUE);
}

BOOL WINAPI wow_menu_client_publish_tree(wow_task_order_process *process, HMENU menu)
{
    return publish_tree(process, 0, menu, FALSE);
}

static BOOL retire_views(wow_task_order_process *process, BOOL borrowed)
{
    wow_user_object_table *table = process->objects;
    BOOL progress, pending;
    ULONG i;
    /* Native recursive destruction is already complete. Withdraw parent
     * views before their referenced children, independent of handle order.
     * This only releases copied storage; it never schedules native destroys
     * or changes original _WOWCleanup's owner/task selection. */
    do {
        progress = pending = FALSE;
        for (i = 0; i <= table->last_handle; ++i) {
            wow_cleanup_handle *entry = &table->entries[i];
            wow_menu_client_object *object;
            DWORD error;
            if (entry->bType != TYPE_MENU ||
                    (entry->pOwner && entry->pOwner != process)) continue;
            object = entry->phead;
            if (!borrowed && IsMenu(object->native)) continue;
            if (object->visiting) { SetLastError(ERROR_BUSY); return FALSE; }
            SetLastError(ERROR_SUCCESS);
            if (!mvdm_softpc_wow_page_domain_retire_menu((ULONG)(ULONG_PTR)object->native)) {
                error = GetLastError();
                if (error != ERROR_BUSY) {
                    SetLastError(error ? error : ERROR_INVALID_STATE);
                    return FALSE;
                }
                pending = TRUE;
                continue;
            }
            ZeroMemory(entry, sizeof(*entry));
            HeapFree(GetProcessHeap(), 0, object);
            progress = TRUE;
        }
    } while (pending && progress);
    if (pending) { SetLastError(ERROR_BUSY); return FALSE; }
    return TRUE;
}

BOOL WINAPI wow_menu_client_forget_borrowed(wow_task_order_process *process)
{
    wow_user_object_table *table;
    ULONG i;
    if (!process || !(table = process->objects)) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    /* Called after all task producers stop. Validate the entire table before
     * withdrawing any view: a failed live-owner check must preserve it. */
    for (i = 0; i <= table->last_handle; ++i) {
        wow_cleanup_handle *entry = &table->entries[i];
        wow_menu_client_object *object = entry->phead;
        if (entry->bType == TYPE_FREE) continue;
        if (entry->bType != TYPE_MENU || entry->pOwner || !object || object->visiting) {
            SetLastError(ERROR_BUSY); return FALSE;
        }
    }
    return retire_views(process, TRUE);
}

BOOL WINAPI wow_menu_client_retire_destroyed(wow_task_order_process *process)
{
    wow_user_object_table *table;
    if (!process || !(table = process->objects)) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    /* Native destruction may recursively destroy submenus; RemoveMenu does
     * not. Test full identities, never retire merely because a root detached.
     * This withdraws invalidated projections, not another task cleanup scan. */
    return retire_views(process, FALSE);
}

BOOL WINAPI wow_menu_client_refresh_all(wow_task_order_process *process)
{
    wow_user_object_table *table;
    ULONG i;
    if (!process || !(table = process->objects)) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    /* Native default processing can change menus without traversing a WOW
     * menu API. Reuse the existing object domain; no second menu/window list
     * or native lifetime policy is introduced here. Query calls do not enter
     * guest code. Publish live links before collecting dead backing. */
    for (i = 0; i <= table->last_handle; ++i) {
        HWND window = table->windows[i];
        WINDOWINFO info = {sizeof(info)};
        HMENU menu;
        if (table->entries[i].bType != TYPE_WINDOW || !window) continue;
        if (!GetWindowInfo(window, &info)) return FALSE;
        if (info.dwStyle & WS_CHILD) continue;
        menu = GetMenu(window);
        if (menu && !publish_tree(process, 0, menu, FALSE)) return FALSE;
        if (!mvdm_softpc_wow_page_domain_window_menu((ULONG)(ULONG_PTR)window,
                (ULONG)(ULONG_PTR)menu)) {
            SetLastError(ERROR_INVALID_STATE); return FALSE;
        }
    }
    for (i = 0; i <= table->last_handle; ++i) {
        wow_cleanup_handle *entry = &table->entries[i];
        wow_menu_client_object *object;
        if (entry->bType != TYPE_MENU) continue;
        object = entry->phead;
        if (IsMenu(object->native) && !publish_tree(process, 0, object->native, FALSE))
            return FALSE;
    }
    return retire_views(process, FALSE);
}

BOOL WINAPI wow_menu_client_destroy_object(wow_task_order_process *process,
    wow_cleanup_handle *entry)
{
    wow_menu_client_object *object;
    if (!process || !entry || entry->bType != TYPE_MENU ||
            entry->pOwner != process || !(object = entry->phead) || object->visiting) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    if (IsMenu(object->native) && !DestroyMenu(object->native)) return FALSE;
    /* Original cleanup can destroy a submenu rather than its root. Publish
     * surviving native parent/window links before releasing copied backing,
     * just as the ordinary native-call return boundary does. */
    return wow_menu_client_refresh_all(process);
}

void WINAPI wow_menu_client_cleanup(wow_cleanup_context *context,
    wow_cleanup_handle *entry)
{
    /* _WOWCleanup selects by process and creator task. This is only its
     * native destruction leaf; failed release must reach the caller. */
    if (!context->thread || !wow_menu_client_destroy_object(
            context->thread->ppi, entry))
        context->object_release_failed = TRUE;
}
