#include "wow_user_object_bindings.h"
#include "wow_window_dispatch_bindings.h"
#include "wow_class_lookup_bindings.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

/* Original client handle query over the existing object domain. The source
 * owns missing/dead-handle policy; this context supplies storage only. */
typedef wow_cleanup_handle *PHE;
static const struct { ULONG cHandleEntries; } original_handle_limits = {0x10000u};
static __declspec(thread) struct { PHE aheList; } original_shared_handles;
#define gpsi (&original_handle_limits)
#define gSharedInfo original_shared_handles
#define HMIndexFromHandle(handle) ((DWORD)(WORD)(handle))
#define GetFullUserHandle wow_original_full_user_handle
#include "opennt-host/windows/core/ntuser/client/handles.c"
#undef GetFullUserHandle
#undef HMIndexFromHandle
#undef gSharedInfo
#undef gpsi

DWORD WINAPI wow_user_full_handle(wow_user_object_table *table, WORD handle)
{
    PHE previous;
    DWORD result;
    if (!table) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    previous = original_shared_handles.aheList;
    original_shared_handles.aheList = table->entries;
    result = wow_original_full_user_handle(handle);
    original_shared_handles.aheList = previous;
    return result;
}

BOOL WINAPI wow_user_window_sample_links(HWND window, wow_user_window_links *output)
{
    wow_user_window_links links = {0};
    WINDOWINFO before = {sizeof(before)}, after = {sizeof(after)};
    DWORD saved = GetLastError();
    if (!window || !output) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!GetWindowInfo(window, &before)) return FALSE;
    links.window = window;
    /* Original WND fields are distinct: getset.c maps a top-level parent
     * query to spwndOwner, but the stored spwndParent is still the desktop.
     * GetParent therefore cannot produce this shared-view field. */
    if (window != GetDesktopWindow()) {
        links.next = GetWindow(window, GW_HWNDNEXT);
        links.parent = GetAncestor(window, GA_PARENT);
        links.owner = GetWindow(window, GW_OWNER);
    }
    links.child = GetWindow(window, GW_CHILD);
    /* Public USER exposes no NT4 thread lock over the native tree. Reject
     * an observed disappearance/change rather than publish a successful
     * empty node. This is a bounded sample, not a desktop-wide atomicity
     * promise; the graph owner must resolve every sampled full identity. */
    if (!GetWindowInfo(window, &after)) return FALSE;
    if (before.atomWindowType != after.atomWindowType ||
            before.dwStyle != after.dwStyle ||
            links.child != GetWindow(window, GW_CHILD) ||
            (window != GetDesktopWindow() &&
             (links.next != GetWindow(window, GW_HWNDNEXT) ||
              links.parent != GetAncestor(window, GA_PARENT) ||
              links.owner != GetWindow(window, GW_OWNER)))) {
        SetLastError(ERROR_RETRY); return FALSE;
    }
    *output = links;
    SetLastError(saved);
    return TRUE;
}

/* The index is traversal scratch only. It uses USER's existing low-word
 * namespace and rejects differing generations; it neither allocates handles
 * nor survives the call as another object registry. */
static BOOL append_sample(HWND window, ULONG *seen,
    wow_user_window_links **rows, ULONG *count, ULONG *capacity)
{
    ULONG index = LOWORD(window), previous;
    wow_user_window_links *grown;
    if (!window) return TRUE;
    previous = seen[index];
    if (previous) {
        if ((*rows)[previous - 1].window == window) return TRUE;
        SetLastError(ERROR_RETRY); return FALSE;
    }
    if (*count == *capacity) {
        ULONG size = *capacity ? *capacity * 2u : 64u;
        if (size > 0x10000u) size = 0x10000u;
        if (size == *capacity) {
            SetLastError(ERROR_INVALID_DATA); return FALSE;
        }
        grown = *rows ? HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
            *rows, size * sizeof(**rows)) :
            HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size * sizeof(**rows));
        if (!grown) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
        *rows = grown;
        *capacity = size;
    }
    (*rows)[*count].window = window;
    seen[index] = ++*count;
    return TRUE;
}

BOOL WINAPI wow_user_window_sample_graph(const HWND *roots, ULONG root_count,
    wow_user_window_links **output, ULONG *output_count)
{
    wow_user_window_links *rows = NULL;
    ULONG *seen, count = 0, capacity = 0, i;
    DWORD error, saved = GetLastError();
    if (!output || !output_count || !roots || !root_count ||
            root_count > 0x10000u) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    seen = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x10000u * sizeof(*seen));
    if (!seen) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    for (i = 0; i < root_count; ++i) {
        if (!roots[i]) { SetLastError(ERROR_INVALID_WINDOW_HANDLE); goto fail; }
        if (!append_sample(roots[i], seen, &rows, &count, &capacity)) goto fail;
    }
    for (i = 0; i < count; ++i) {
        wow_user_window_links links;
        if (!wow_user_window_sample_links(rows[i].window, &links)) goto fail;
        rows[i] = links;
        /* append_sample can move rows. Never retain a pointer into it. */
        if (!append_sample(links.next, seen, &rows, &count, &capacity) ||
            !append_sample(links.parent, seen, &rows, &count, &capacity) ||
            !append_sample(links.child, seen, &rows, &count, &capacity) ||
            !append_sample(links.owner, seen, &rows, &count, &capacity)) goto fail;
    }
    /* Native USER may change while the closure is collected. Recheck every
     * edge, without executing callbacks or guest code, before handing it to
     * the publisher. This detects observed races, not a substitute USER lock. */
    for (i = 0; i < count; ++i) {
        wow_user_window_links links;
        if (!wow_user_window_sample_links(rows[i].window, &links)) goto fail;
        if (links.next != rows[i].next || links.parent != rows[i].parent ||
            links.child != rows[i].child || links.owner != rows[i].owner) {
            SetLastError(ERROR_RETRY); goto fail;
        }
    }
    HeapFree(GetProcessHeap(), 0, seen);
    *output = rows;
    *output_count = count;
    SetLastError(saved);
    return TRUE;
fail:
    error = GetLastError();
    if (rows) HeapFree(GetProcessHeap(), 0, rows);
    HeapFree(GetProcessHeap(), 0, seen);
    SetLastError(error);
    return FALSE;
}

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
