#include "wow_cleanup_bindings.h"
#include "wow_window_words_binding.h"
#include "wow_window_procedure_entry.h"

void WINAPI wow_cleanup_set_class_procedure(wow_class_lookup_entry *entry,
    WNDPROC procedure)
{
    wow_class_words_restore_procedure((wow_class_words_binding *)entry, procedure);
}

void WINAPI wow_cleanup_set_window_procedure(wow_cleanup_context *context,
    wow_cleanup_window *view, WNDPROC procedure)
{
    DWORD error = GetLastError(), pid;
    wow_window_words_binding *binding = NULL;
    BOOL valid;
    if (!view || !view->native_window || !procedure ||
            !GetWindowThreadProcessId(view->native_window, &pid) ||
            pid != GetCurrentProcessId()) {
        context->object_release_failed = TRUE;
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return;
    }
    if (view->storage) {
        binding = wow_window_words_acquire(view->native_window);
        valid = binding == view->storage;
        wow_window_words_release(binding);
        if (!valid) {
            context->object_release_failed = TRUE;
            SetLastError(ERROR_INVALID_STATE); return;
        }
    }
    /* The non-xxx original cleanup cannot call application code. Native
     * GWL_WNDPROC assignment is the finite no-callout storage operation;
     * style/layout/message services do not belong in this leaf. */
    SetLastError(ERROR_SUCCESS);
    if (!SetWindowLongA(view->native_window, GWL_WNDPROC, (LONG)procedure)) {
        if (!GetLastError()) SetLastError(ERROR_INVALID_STATE);
        context->object_release_failed = TRUE;
        return;
    }
    view->lpfnWndProc = procedure;
    view->procedure_is_guest = FALSE;
    SetLastError(error);
}

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
        {
            wow_window_dispatch_target target;
            WNDPROC native = (WNDPROC)GetWindowLongA(window_handle, GWL_WNDPROC);
            if (!native) return FALSE;
            window->procedure_is_guest = wow_window_entry_guest_target(native, &target);
            window->lpfnWndProc = window->procedure_is_guest ?
                (WNDPROC)target.procedure : native;
            window->native_window = window_handle;
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
    if (!wow_window_words_collect_retired(objects)) return FALSE;
    desktop=context->thread->rpdesk->pDeskInfo;
    if (!build_tree(objects,&root)) { clear_tree(objects); return FALSE; }
    previous_root=desktop->spwnd;
    context->handles.aheList=objects->entries;
    context->last_handle=objects->last_handle;
    desktop->spwnd=&root;
    context->object_release_failed=FALSE;
    __try {
        result=_WOWCleanup(instance,task,selectors,count,context);
        if (context->object_release_failed) result=FALSE;
    } __finally {
        desktop->spwnd=previous_root;
        clear_tree(objects);
    }
    return result;
}
