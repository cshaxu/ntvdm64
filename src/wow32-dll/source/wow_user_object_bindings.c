#include "wow_user_object_bindings.h"
#include "wow_window_dispatch_bindings.h"
#include "wow_class_lookup_bindings.h"
#include "wow_user_runtime.h"
#include "wow_task_order_bindings.h"
#include "wow_window_words_binding.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

/* Original query reads shared publication, never the cleanup ownership table.
 * One copied metadata row suffices; no native pointer, table-sized copy or
 * additional handle registry is required. The original body cannot reenter. */
typedef wow_cleanup_handle *PHE;
static const struct { ULONG cHandleEntries; } original_handle_limits = {0x10000u};
static __declspec(thread) PHE original_shared_entry;
#define gpsi (&original_handle_limits)
#define WowSharedHandleEntry(index) (original_shared_entry)
#define HMIndexFromHandle(handle) ((DWORD)(WORD)(handle))
#define GetFullUserHandle wow_original_full_user_handle
#include "opennt-host/windows/core/ntuser/client/handles.c"
#undef GetFullUserHandle
#undef HMIndexFromHandle
#undef WowSharedHandleEntry
#undef gpsi

DWORD WINAPI wow_user_full_handle(wow_user_object_table *table, WORD handle)
{
    PHE previous;
    wow_cleanup_handle entry = {0};
    DWORD result, metadata;
    if (!table) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    if (!mvdm_softpc_wow_page_domain_handle_metadata(handle, &metadata)) {
        SetLastError(ERROR_INVALID_STATE); return 0;
    }
    entry.bType = (BYTE)metadata;
    entry.bFlags = (BYTE)(metadata >> 8);
    entry.wUniq = HIWORD(metadata);
    previous = original_shared_entry;
    original_shared_entry = &entry;
    result = wow_original_full_user_handle(handle);
    original_shared_entry = previous;
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

static BOOL sampled_links_match(const wow_user_window_links *row)
{
    wow_user_window_links links;
    if (!wow_user_window_sample_links(row->window, &links)) return FALSE;
    if (links.next != row->next || links.parent != row->parent ||
            links.child != row->child || links.owner != row->owner) {
        SetLastError(ERROR_RETRY); return FALSE;
    }
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
        if (!sampled_links_match(&rows[i])) goto fail;
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

BOOL WINAPI wow_user_window_refresh_graph(HWND callback_window)
{
    wow_user_runtime_thread *binding;
    wow_user_object_table *objects;
    HWND *roots, desktop;
    ULONG count = 0, i;
    BOOL result;
    DWORD error, saved = GetLastError();
    /* Native-only callers outside a guest page domain have no copied view.
     * An active domain, however, must have its real process owner. */
    if (!mvdm_softpc_wow_page_domain_active()) return TRUE;
    binding = wow_user_runtime_current();
    if (!binding || !binding->thread || !binding->thread->ppi ||
            !(objects = binding->thread->ppi->objects) ||
            objects->last_handle >= 0x10000u) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    if (!wow_window_words_collect_retired(objects)) return FALSE;
    roots = HeapAlloc(GetProcessHeap(), 0,
        (objects->last_handle + 3u) * sizeof(*roots));
    if (!roots) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    desktop = GetDesktopWindow();
    roots[count++] = desktop;
    if (callback_window && callback_window != desktop)
        roots[count++] = callback_window;
    /* createw.c links the window after WM_NCCREATE, and unlinks it before
     * WM_NCDESTROY. Preserve those native intermediate states rather than
     * inventing an incoming parent edge just to make the window reachable. */
    for (i = 1; i <= objects->last_handle; ++i) {
        HWND window = objects->windows[i];
        if (objects->entries[i].bType != TYPE_WINDOW || !window ||
                window == desktop || window == callback_window) continue;
        roots[count++] = window;
    }
    result = wow_user_window_project_graph(roots, count);
    error = result ? saved : GetLastError();
    HeapFree(GetProcessHeap(), 0, roots);
    SetLastError(error);
    return result;
}

BOOL WINAPI wow_user_window_project_graph(const HWND *roots, ULONG root_count)
{
    wow_user_window_links *rows = NULL;
    mvdm_softpc_wow_window_links *links = NULL;
    mvdm_softpc_wow_window_descriptor *nodes = NULL;
    ULONG count = 0, i;
    BOOL result = FALSE;
    DWORD error, saved = GetLastError();
    if (!wow_user_window_sample_graph(roots, root_count, &rows, &count)) return FALSE;
    nodes = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, count * sizeof(*nodes));
    links = HeapAlloc(GetProcessHeap(), 0, count * sizeof(*links));
    if (!nodes || !links) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto done; }
    /* Sample every descriptor before publishing any new node. These are
     * public native queries, not class registration or procedure ownership. */
    for (i = 0; i < count; ++i) {
        int length;
        WINDOWINFO info = {sizeof(info)};
        if (!GetWindowInfo(rows[i].window, &info)) goto done;
        nodes[i].atom = info.atomWindowType;
        length = GetClassNameA(rows[i].window, nodes[i].name, sizeof(nodes[i].name));
        if (!length) goto done;
        if (length >= sizeof(nodes[i].name) - 1) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER); goto done;
        }
        nodes[i].name_bytes = (ULONG)length + 1u;
        SetLastError(ERROR_SUCCESS);
        nodes[i].class_style = GetClassLongA(rows[i].window, GCL_STYLE);
        if (!nodes[i].class_style && GetLastError() != ERROR_SUCCESS) goto done;
        SetLastError(ERROR_SUCCESS);
        nodes[i].window_extra = GetClassLongA(rows[i].window, GCL_CBWNDEXTRA);
        if (!nodes[i].window_extra && GetLastError() != ERROR_SUCCESS) goto done;
        links[i].window = (ULONG)(ULONG_PTR)rows[i].window;
        links[i].next = (ULONG)(ULONG_PTR)rows[i].next;
        links[i].parent = (ULONG)(ULONG_PTR)rows[i].parent;
        links[i].child = (ULONG)(ULONG_PTR)rows[i].child;
        links[i].owner = (ULONG)(ULONG_PTR)rows[i].owner;
    }
    /* The worker prepares all storage and revalidates native relationships
     * before one commit. Do not publish nodes sequentially in this caller. */
    if (!mvdm_softpc_wow_page_domain_project_window_graph(count, links, nodes)) {
        SetLastError(ERROR_INVALID_STATE); goto done;
    }
    result = TRUE;
done:
    error = result ? saved : GetLastError();
    if (links) HeapFree(GetProcessHeap(), 0, links);
    if (nodes) HeapFree(GetProcessHeap(), 0, nodes);
    HeapFree(GetProcessHeap(), 0, rows);
    SetLastError(error);
    return result;
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
            object->pcls ? object->pcls->guest_server : 0u, object->adwWOW)) {
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
