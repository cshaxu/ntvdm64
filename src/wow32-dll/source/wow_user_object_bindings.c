#include "wow_user_object_bindings.h"
#include "wow_window_dispatch_bindings.h"
#include "wow_class_lookup_bindings.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

BOOL WINAPI wow_user_window_publish(wow_user_object_table *table, HWND window,
    struct wow_cleanup_window *object, struct wow_task_order_thread *thread)
{
    WORD index=LOWORD(window);
    wow_cleanup_handle *entry;
    if (!table || !object || !thread || !index || index==0xffff) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    entry=&table->entries[index];
    if (entry->bType!=TYPE_FREE) {
        SetLastError(ERROR_ALREADY_EXISTS); return FALSE;
    }
    entry->phead=object; entry->pOwner=thread;
    entry->wUniq=HIWORD(window); entry->bFlags=0;
    entry->bType=TYPE_WINDOW;
    if (!mvdm_softpc_wow_page_domain_publish_handle(index, entry->wUniq,
            entry->bType, entry->bFlags, (unsigned long)(ULONG_PTR)window,
            object->pcls ? object->pcls->guest_server : 0u)) {
        ZeroMemory(entry,sizeof(*entry));
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    table->windows[index]=window;
    if (index>table->last_handle) table->last_handle=index;
    return TRUE;
}

BOOL WINAPI wow_user_window_retire(wow_user_object_table *table, HWND window,
    struct wow_cleanup_window *object, ULONG *retained_backing)
{
    wow_cleanup_handle *entry;
    if (!table || !object) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    entry=&table->entries[LOWORD(window)];
    if (entry->bType!=TYPE_WINDOW || entry->wUniq!=HIWORD(window) || entry->phead!=object) {
        SetLastError(ERROR_NOT_FOUND); return FALSE;
    }
    if (!mvdm_softpc_wow_page_domain_retire_handle(LOWORD(window), retained_backing)) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    ZeroMemory(entry,sizeof(*entry));
    table->windows[LOWORD(window)]=NULL;
    return TRUE;
}

HWND WINAPI wow_user_window_full_handle(wow_user_object_table *table,
    WORD index)
{
    wow_cleanup_handle *entry;

    if (!table || !index || index == 0xffffu) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    entry = &table->entries[index];
    if (entry->bType != TYPE_WINDOW || !entry->phead ||
            LOWORD(table->windows[index]) != index) {
        SetLastError(ERROR_NOT_FOUND);
        return NULL;
    }
    return table->windows[index];
}
