#include "wow_cleanup_bindings.h"

static void clear_tree(wow_user_object_table *table)
{
    ULONG i;
    for (i=0;i<=table->last_handle;++i) {
        if (table->entries[i].bType==TYPE_WINDOW) {
            wow_cleanup_window *window=table->entries[i].phead;
            window->spwndChild=window->spwndNext=NULL;
        }
    }
}

static BOOL build_tree(wow_user_object_table *table, wow_cleanup_window *root)
{
    ULONG i;
    clear_tree(table);
    for (i=0;i<=table->last_handle;++i) {
        wow_cleanup_handle *entry=&table->entries[i];
        wow_cleanup_window *window, *parent_view=root;
        HWND window_handle, parent;
        if (entry->bType!=TYPE_WINDOW) continue;
        window=entry->phead;
        window_handle=(HWND)(ULONG_PTR)(i|((DWORD)entry->wUniq<<16));
        if (!IsWindow(window_handle)) {
            SetLastError(ERROR_INVALID_WINDOW_HANDLE); return FALSE;
        }
        /* GetAncestor(GA_PARENT), unlike GetParent, does not treat the owner
         * of a popup as its parent. Contract away native-only intermediate
         * nodes: they have no enrolled CLS/WOW procedure to neutralize.
         * The original PseudoDestroyClassWindows only changes matching live
         * procedures; no callbacks or order-sensitive operations occur. */
        parent=GetAncestor(window_handle,GA_PARENT);
        while (parent) {
            wow_cleanup_handle *candidate=&table->entries[LOWORD(parent)];
            if (candidate->bType==TYPE_WINDOW && candidate->wUniq==HIWORD(parent)) {
                parent_view=candidate->phead;
                break;
            }
            parent=GetAncestor(parent,GA_PARENT);
        }
        window->spwndNext=parent_view->spwndChild;
        parent_view->spwndChild=window;
    }
    return TRUE;
}

BOOL WINAPI wow_user_cleanup_bound(HANDLE instance, DWORD task, PNEMODULESEG selectors,
    DWORD count, wow_cleanup_context *context)
{
    wow_cleanup_window root={0}, *previous_root;
    wow_user_object_table *objects;
    wow_cleanup_desktop_info *desktop;
    BOOL result = FALSE;
    if (!context || !context->thread || !context->thread->ppi ||
        !context->thread->ppi->objects || !context->thread->rpdesk ||
        !context->thread->rpdesk->pDeskInfo) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    objects=context->thread->ppi->objects;
    desktop=context->thread->rpdesk->pDeskInfo;
    if (!build_tree(objects,&root)) { clear_tree(objects); return FALSE; }
    previous_root=desktop->spwnd;
    context->handles.aheList=objects->entries;
    context->last_handle=objects->last_handle;
    desktop->spwnd=&root;
    __try {
        result=_WOWCleanup(instance,task,selectors,count,context);
    } __finally {
        desktop->spwnd=previous_root;
        clear_tree(objects);
    }
    return result;
}
