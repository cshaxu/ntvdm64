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
    WINDOWINFO before = {sizeof(before)};
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
    /* These are observations, not a native desktop lock. The composed
     * transaction validates references/chains, not later external freshness. */
    *output = links;
    SetLastError(saved);
    return TRUE;
}

static BOOL project_graph(const HWND *, ULONG, HWND);

static BOOL refresh_graph(HWND callback_window, HWND returned_window)
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
    result = project_graph(roots, count, returned_window);
    error = result ? saved : GetLastError();
    HeapFree(GetProcessHeap(), 0, roots);
    SetLastError(error);
    return result;
}

BOOL WINAPI wow_user_window_refresh_graph(HWND callback_window)
{
    return refresh_graph(callback_window, NULL);
}

BOOL WINAPI wow_user_window_refresh_return(HWND returned_window)
{
    return refresh_graph(NULL, returned_window);
}

typedef mvdm_softpc_wow_window_observation window_observation;

/* A public USER sample has no desktop lock. Capture a single identity and
 * its fields; only the identity itself is checked again, never neighbours. */
static BOOL capture_window(HWND window, window_observation *row)
{
    WINDOWINFO info = {sizeof(info)};
    wow_user_window_links links;
    DWORD pid = 0, tid;
    int length;
    ZeroMemory(row, sizeof(*row));
    row->thread_id = GetWindowThreadProcessId(window, &row->process_id);
    if (!row->thread_id || !GetWindowInfo(window, &info) ||
            !wow_user_window_sample_links(window, &links)) return FALSE;
    row->links.window = (ULONG)(ULONG_PTR)window;
    row->links.next = (ULONG)(ULONG_PTR)links.next;
    row->links.parent = (ULONG)(ULONG_PTR)links.parent;
    row->links.child = (ULONG)(ULONG_PTR)links.child;
    row->links.owner = (ULONG)(ULONG_PTR)links.owner;
    CopyMemory(row->window_rect, &info.rcWindow, sizeof(RECT));
    CopyMemory(row->client_rect, &info.rcClient, sizeof(RECT));
    row->style = info.dwStyle; row->exstyle = info.dwExStyle;
    row->control_id = (row->style & WS_CHILD) ? GetWindowLongA(window, GWL_ID) : 0;
    row->descriptor.atom = info.atomWindowType;
    length = GetClassNameA(window, row->descriptor.name, sizeof(row->descriptor.name));
    if (!length) return FALSE;
    if (length >= sizeof(row->descriptor.name) - 1) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER); return FALSE;
    }
    row->descriptor.name_bytes = (ULONG)length + 1u;
    SetLastError(ERROR_SUCCESS);
    row->descriptor.class_style = GetClassLongA(window, GCL_STYLE);
    if (!row->descriptor.class_style && GetLastError()) return FALSE;
    SetLastError(ERROR_SUCCESS);
    row->descriptor.window_extra = GetClassLongA(window, GCL_CBWNDEXTRA);
    if (!row->descriptor.window_extra && GetLastError()) return FALSE;
    tid = GetWindowThreadProcessId(window, &pid);
    if (tid != row->thread_id || pid != row->process_id) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return FALSE;
    }
    return TRUE;
}

/* This index/array is transaction scratch; the page-domain remains the sole
 * retained observation owner. Full generations never alias on a low word. */
static BOOL put_observation(window_observation **rows, ULONG *count,
    ULONG *capacity, ULONG *index, const window_observation *value)
{
    ULONG slot = LOWORD(value->links.window), at = index[slot];
    window_observation *grown;
    if (!slot || slot == 0xffffu) { SetLastError(ERROR_INVALID_DATA); return FALSE; }
    if (at) {
        window_observation *previous = &(*rows)[at - 1u];
        if (previous->links.window != value->links.window &&
                previous->process_id == GetCurrentProcessId() &&
                IsWindow((HWND)(ULONG_PTR)previous->links.window)) {
            SetLastError(ERROR_INVALID_STATE); return FALSE;
        }
        *previous = *value;
        return TRUE;
    }
    if (*count == *capacity) {
        ULONG size = *capacity ? *capacity * 2u : 64u;
        if (size > 0xfffeu) size = 0xfffeu;
        if (size <= *capacity) { SetLastError(ERROR_INVALID_DATA); return FALSE; }
        grown = *rows ? HeapReAlloc(GetProcessHeap(), 0, *rows, size * sizeof(**rows)) :
            HeapAlloc(GetProcessHeap(), 0, size * sizeof(**rows));
        if (!grown) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
        *rows = grown; *capacity = size;
    }
    (*rows)[*count] = *value;
    index[slot] = ++*count;
    return TRUE;
}

static ULONG *observation_edge(window_observation *row, unsigned edge)
{
    if (edge == 0) return &row->links.next;
    if (edge == 1) return &row->links.parent;
    if (edge == 2) return &row->links.owner;
    return &row->links.child;
}

/* Native concurrent sampling can join an old external edge to a new local
 * chain. Break only an external edge of such a cycle. Local-only cycles are
 * integration errors. Gray/black traversal is linear, not a stability retry. */
static BOOL normalize_observation_chains(window_observation *rows, ULONG count,
    const ULONG *index)
{
    BYTE *state = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, count);
    ULONG start, at, next, cut;
    unsigned edge;
    DWORD pid = GetCurrentProcessId();
    if (!state) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    for (edge = 0; edge < 3u; ++edge) {
        ZeroMemory(state, count);
        for (start = 0; start < count; ++start) {
            at = start;
            while (!state[at]) {
                state[at] = 1;
                next = *observation_edge(&rows[at], edge);
                if (!next) break;
                at = index[LOWORD(next)] - 1u;
            }
            if (state[at] == 1 && *observation_edge(&rows[at], edge)) {
                cut = at;
                do {
                    if (rows[cut].process_id != pid) break;
                    cut = index[LOWORD(*observation_edge(&rows[cut], edge))] - 1u;
                } while (cut != at);
                if (rows[cut].process_id == pid) {
                    HeapFree(GetProcessHeap(), 0, state);
                    SetLastError(ERROR_INVALID_DATA); return FALSE;
                }
                /* Mark the whole cycle before cutting it; the now-detached
                 * remainder must not be mistaken for another gray cycle. */
                next = at;
                do {
                    state[next] = 2;
                    next = index[LOWORD(*observation_edge(&rows[next], edge))] - 1u;
                } while (next != at);
                *observation_edge(&rows[cut], edge) = 0;
            }
            at = start;
            while (state[at] == 1) {
                state[at] = 2;
                next = *observation_edge(&rows[at], edge);
                if (!next) break;
                at = index[LOWORD(next)] - 1u;
            }
        }
    }
    HeapFree(GetProcessHeap(), 0, state);
    return TRUE;
}

static BOOL project_graph(const HWND *roots, ULONG root_count, HWND returned_window)
{
    window_observation *rows = NULL, captured;
    ULONG *index = NULL, count = 0, capacity = 0, i, used;
    BOOL result = FALSE;
    DWORD error, pid = GetCurrentProcessId(), saved = GetLastError();
    if (!roots || !root_count || root_count >= 0x10000u) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    for (i = 0; i < root_count; ++i) if (!roots[i]) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return FALSE;
    }
    if (!mvdm_softpc_wow_page_domain_copy_window_view(0, NULL, &count)) return FALSE;
    capacity = count;
    rows = HeapAlloc(GetProcessHeap(), 0, (count ? count : 1u) * sizeof(*rows));
    index = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x10000u * sizeof(*index));
    if (!rows || !index) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto done; }
    if (!mvdm_softpc_wow_page_domain_copy_window_view(capacity, rows, &count)) goto done;
    for (i = used = 0; i < count; ++i) {
        HWND window = (HWND)(ULONG_PTR)rows[i].links.window;
        if (capture_window(window, &captured)) rows[i] = captured;
        else if (!IsWindow(window)) {
            /* Withdraw expired observations with their incoming links. This
             * includes local native controls without a WW cleanup owner. */
            continue;
        } else if (rows[i].process_id == pid) {
            goto done; /* Live local state must not fall back to old fields. */
        } else if (!rows[i].descriptor.atom) goto done; /* Cold desktop. */
        rows[used] = rows[i];
        index[LOWORD(rows[used].links.window)] = used + 1u;
        ++used;
    }
    count = used;
    for (i = 0; i < root_count; ++i) {
        if (!capture_window(roots[i], &captured) ||
                !put_observation(&rows, &count, &capacity, index, &captured)) goto done;
    }
    /* A native setter returns its previous window, not its new target. It
     * may disappear during native callbacks or the ownership handoff. Root
     * a still-live full identity in this same transaction; never resurrect
     * a dead return or bind its low word to a replacement generation. */
    if (returned_window) {
        if (capture_window(returned_window, &captured)) {
            if (!put_observation(&rows, &count, &capacity, index, &captured)) goto done;
        } else if (IsWindow(returned_window)) goto done;
    }
    for (i = 0; i < count; ++i) {
        unsigned edge;
        for (edge = 0; edge < 4u; ++edge) {
            ULONG target = *observation_edge(&rows[i], edge), found;
            if (!target) continue;
            found = index[LOWORD(target)];
            if (found && rows[found - 1u].links.window == target) continue;
            if (capture_window((HWND)(ULONG_PTR)target, &captured)) {
                if (!put_observation(&rows, &count, &capacity, index, &captured)) goto done;
            } else {
                /* A vanished external endpoint is unavailable, not an alias
                 * for a reused slot. No native hierarchy is mutated here. */
                if (IsWindow((HWND)(ULONG_PTR)target)) {
                    DWORD owner = 0;
                    GetWindowThreadProcessId((HWND)(ULONG_PTR)target, &owner);
                    if (owner == pid) goto done;
                }
                *observation_edge(&rows[i], edge) = 0;
            }
        }
    }
    /* A replacement generation may have displaced an earlier observation
     * during closure. Only observation edges may retain that old identity. */
    for (i = 0; i < count; ++i) {
        unsigned edge;
        for (edge = 0; edge < 4u; ++edge) {
            ULONG *target = observation_edge(&rows[i], edge), found;
            if (!*target) continue;
            found = index[LOWORD(*target)];
            if (found && rows[found - 1u].links.window == *target) continue;
            if (rows[i].process_id == pid) { SetLastError(ERROR_INVALID_STATE); goto done; }
            *target = 0;
        }
    }
    if (!normalize_observation_chains(rows, count, index)) goto done;
    result = mvdm_softpc_wow_page_domain_project_window_graph(count, rows);
done:
    error = result ? saved : GetLastError();
    if (index) HeapFree(GetProcessHeap(), 0, index);
    if (rows) HeapFree(GetProcessHeap(), 0, rows);
    SetLastError(error);
    return result;
}

BOOL WINAPI wow_user_window_project_graph(const HWND *roots, ULONG root_count)
{
    return project_graph(roots, root_count, NULL);
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
