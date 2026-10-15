#include "wow_menu_client_bindings.h"
#include "wow_cleanup_bindings.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

/* Original cleanup.c reads hTaskWow through the adapted PROCOBJHEAD first
 * member. The original process-owned scan remains the destruction policy;
 * this record is not a second USER menu or a native-layout overlay. */
typedef struct wow_menu_client_object {
    DWORD hTaskWow;
    HMENU native;
    BOOL local_state; /* Freshness responsibility, never native destruction ownership. */
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

typedef struct menu_capture_row {
    mvdm_softpc_wow_menu_observation view;
    wow_menu_client_object *object;
    BOOL created, registering;
} menu_capture_row;

typedef struct menu_capture {
    wow_task_order_process *process;
    ULONG *index, count, capacity, item_count, item_capacity;
    menu_capture_row *rows;
    mvdm_softpc_wow_menu_query_item *items;
} menu_capture;

static BOOL reserve_capture(void **buffer, ULONG *capacity, ULONG needed, SIZE_T size)
{
    ULONG next;
    void *grown;
    if (needed <= *capacity) return TRUE;
    next = *capacity && *capacity <= MAXULONG / 2u ? *capacity * 2u : needed;
    if (next < needed) next = needed;
    if (next > (SIZE_T)-1 / size) { SetLastError(ERROR_ARITHMETIC_OVERFLOW); return FALSE; }
    grown = *buffer ? HeapReAlloc(GetProcessHeap(), 0, *buffer, next * size) :
        HeapAlloc(GetProcessHeap(), 0, next * size);
    if (!grown) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    *buffer = grown; *capacity = next;
    return TRUE;
}

static BOOL capture_tree(menu_capture *capture, DWORD task, HMENU menu, BOOL registering,
    BOOL local_state)
{
    wow_user_object_table *table = capture->process->objects;
    wow_cleanup_handle *entry;
    wow_menu_client_object *object = NULL;
    mvdm_softpc_wow_menu_query_item *items = NULL;
    ULONG value = (ULONG)(ULONG_PTR)menu, at;
    WORD slot = LOWORD(value);
    int count, i;
    BOOL created = FALSE, result = FALSE;
    DWORD error;
    if (!slot || slot == 1u || slot == 0xffffu || !IsMenu(menu)) {
        SetLastError(ERROR_INVALID_MENU_HANDLE); return FALSE;
    }
    at = capture->index[slot];
    if (at) {
        if (at == MAXULONG || capture->rows[at - 1u].view.menu != value || registering) {
            SetLastError(ERROR_INVALID_DATA); return FALSE;
        }
        return TRUE;
    }
    entry = &table->entries[slot];
    if (entry->bType == TYPE_FREE) {
        object = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*object));
        if (!object) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
        object->hTaskWow = registering ? task : 0;
        object->native = menu;
        created = TRUE;
    } else {
        menu = wow_menu_client_full_handle(table, menu);
        if (!menu || (entry->pOwner && entry->pOwner != capture->process)) {
            SetLastError(ERROR_INVALID_MENU_HANDLE); return FALSE;
        }
        if (registering) { SetLastError(ERROR_ALREADY_EXISTS); return FALSE; }
        object = entry->phead;
    }
    local_state = local_state || object->local_state;
    capture->index[slot] = MAXULONG; /* Cycle guard; no persistent owner mutation. */
    count = GetMenuItemCount(menu);
    if (count < 0) goto done;
    if ((SIZE_T)count > (SIZE_T)-1 / sizeof(*items)) {
        SetLastError(ERROR_ARITHMETIC_OVERFLOW); goto done;
    }
    if (count) {
        items = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, count * sizeof(*items));
        if (!items) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto done; }
    }
    for (i = 0; i < count; ++i) {
        MENUITEMINFOW info = {sizeof(info)};
        info.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
        if (!GetMenuItemInfoW(menu, i, TRUE, &info)) goto done;
        if (info.hSubMenu && !capture_tree(capture, 0, info.hSubMenu, FALSE, local_state)) goto done;
        items[i].type = info.fType; items[i].state = info.fState;
        items[i].id = info.wID; items[i].submenu = (ULONG)(ULONG_PTR)info.hSubMenu;
    }
    if ((ULONG)count > MAXULONG - capture->item_count) {
        SetLastError(ERROR_ARITHMETIC_OVERFLOW); goto done;
    }
    if (!reserve_capture((void **)&capture->rows, &capture->capacity,
            capture->count + 1u, sizeof(*capture->rows)) ||
            !reserve_capture((void **)&capture->items, &capture->item_capacity,
                capture->item_count + count, sizeof(*capture->items))) goto done;
    at = capture->count++;
    capture->rows[at].view.menu = (ULONG)(ULONG_PTR)menu;
    capture->rows[at].view.task = object->hTaskWow;
    capture->rows[at].view.first_item = capture->item_count;
    capture->rows[at].view.item_count = count;
    capture->rows[at].view.local_state = local_state;
    capture->rows[at].object = object;
    capture->rows[at].created = created;
    capture->rows[at].registering = registering;
    if (count) CopyMemory(capture->items + capture->item_count, items, count * sizeof(*items));
    capture->item_count += count;
    capture->index[slot] = at + 1u;
    result = TRUE;
done:
    error = GetLastError();
    if (items) HeapFree(GetProcessHeap(), 0, items);
    if (!result && created) HeapFree(GetProcessHeap(), 0, object);
    SetLastError(error);
    return result;
}

static BOOL publish_tree(wow_task_order_process *process,
    DWORD task, HMENU menu, BOOL registering, BOOL local_state)
{
    menu_capture capture = {0};
    mvdm_softpc_wow_menu_observation *views = NULL;
    BOOL result = FALSE;
    ULONG i;
    DWORD error, saved = GetLastError();
    if (!process || !process->objects) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    capture.process = process;
    capture.index = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x10000u * sizeof(ULONG));
    if (!capture.index) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto done; }
    if (!capture_tree(&capture, task, menu, registering, local_state)) goto done;
    views = HeapAlloc(GetProcessHeap(), 0, capture.count * sizeof(*views));
    if (!views) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto done; }
    for (i = 0; i < capture.count; ++i) views[i] = capture.rows[i].view;
    if (!mvdm_softpc_wow_page_domain_publish_menus(capture.count, views,
            capture.item_count, capture.items)) goto done;
    /* All fallible preparation is complete. Install cleanup ownership only
     * after the entire copied tree has committed, with guest entry excluded. */
    for (i = 0; i < capture.count; ++i) {
        menu_capture_row *row = &capture.rows[i];
        WORD slot = LOWORD(row->view.menu);
        wow_cleanup_handle *entry = &process->objects->entries[slot];
        row->object->local_state = row->view.local_state;
        if (!row->created) continue;
        entry->phead = row->object;
        entry->pOwner = row->registering ? process : NULL;
        entry->wUniq = HIWORD(row->view.menu);
        entry->bType = TYPE_MENU;
        if (slot > process->objects->last_handle) process->objects->last_handle = slot;
    }
    result = TRUE;
done:
    error = result ? saved : GetLastError();
    if (!result) for (i = 0; i < capture.count; ++i)
        if (capture.rows[i].created) HeapFree(GetProcessHeap(), 0, capture.rows[i].object);
    if (views) HeapFree(GetProcessHeap(), 0, views);
    if (capture.rows) HeapFree(GetProcessHeap(), 0, capture.rows);
    if (capture.items) HeapFree(GetProcessHeap(), 0, capture.items);
    if (capture.index) HeapFree(GetProcessHeap(), 0, capture.index);
    SetLastError(error);
    return result;
}

BOOL WINAPI wow_menu_client_register_created(wow_task_order_process *process,
    DWORD task, HMENU menu)
{
    if (!task) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    return publish_tree(process, task, menu, TRUE, TRUE);
}

BOOL WINAPI wow_menu_client_publish_tree(wow_task_order_process *process, HMENU menu)
{
    return publish_tree(process, 0, menu, FALSE, FALSE);
}

BOOL WINAPI wow_menu_client_publish_local_tree(wow_task_order_process *process, HMENU menu)
{
    return publish_tree(process, 0, menu, FALSE, TRUE);
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
            SetLastError(ERROR_SUCCESS);
            if (!mvdm_softpc_wow_page_domain_retire_menu((ULONG)(ULONG_PTR)object->native)) {
                error = GetLastError();
                if (!borrowed && !entry->pOwner && !object->local_state &&
                        (error == ERROR_BUSY || error == ERROR_NOT_ENOUGH_MEMORY)) continue;
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
        if (entry->bType != TYPE_MENU || entry->pOwner || !object) {
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
        if (menu && !publish_tree(process, 0, menu, FALSE, TRUE)) return FALSE;
        if (!mvdm_softpc_wow_page_domain_window_menu((ULONG)(ULONG_PTR)window,
                (ULONG)(ULONG_PTR)menu)) {
            SetLastError(ERROR_INVALID_STATE); return FALSE;
        }
    }
    if (mvdm_softpc_wow_page_domain_active()) {
        mvdm_softpc_wow_window_observation *view;
        ULONG count = 0;
        BOOL complete = TRUE;
        DWORD error;
        if (!mvdm_softpc_wow_page_domain_copy_window_view(0, NULL, &count)) return FALSE;
        view = HeapAlloc(GetProcessHeap(), 0, count * sizeof(*view));
        if (!view) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
        if (!mvdm_softpc_wow_page_domain_copy_window_view(count, view, &count)) complete = FALSE;
        for (i = 0; complete && i < count; ++i) {
            HWND window = (HWND)(ULONG_PTR)view[i].links.window;
            BOOL local_state = view[i].process_id == GetCurrentProcessId();
            HMENU menu;
            if (window == GetDesktopWindow() || (view[i].style & WS_CHILD) ||
                    table->windows[LOWORD(window)] == window) continue;
            /* Covers observed native controls/windows as well as other
             * processes. Storage enrollment is not a process-ownership test. */
            menu = GetMenu(window);
            if (menu && !publish_tree(process, 0, menu, FALSE, local_state)) {
                if (local_state) complete = FALSE;
                continue; /* Retain a complete external tree/attachment. */
            }
            if (!mvdm_softpc_wow_page_domain_window_menu(view[i].links.window,
                    (ULONG)(ULONG_PTR)menu)) {
                SetLastError(ERROR_INVALID_STATE); complete = FALSE;
            }
        }
        error = GetLastError();
        HeapFree(GetProcessHeap(), 0, view);
        SetLastError(error);
        if (!complete) return FALSE;
    }
    for (i = 0; i <= table->last_handle; ++i) {
        wow_cleanup_handle *entry = &table->entries[i];
        wow_menu_client_object *object;
        if (entry->bType != TYPE_MENU) continue;
        object = entry->phead;
        if (IsMenu(object->native) && !publish_tree(process, 0, object->native, FALSE,
                object->local_state) && (entry->pOwner || object->local_state)) return FALSE;
        /* Borrowed unattached menus may belong to another process. Local
         * attachments above and explicit callback/mutation roots are required;
         * optional observations retain their last complete tree on failure. */
    }
    return retire_views(process, FALSE);
}

BOOL WINAPI wow_menu_client_destroy_object(wow_task_order_process *process,
    wow_cleanup_handle *entry)
{
    wow_menu_client_object *object;
    if (!process || !entry || entry->bType != TYPE_MENU ||
            entry->pOwner != process || !(object = entry->phead)) {
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
