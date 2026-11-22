#include <windows.h>
#include <winternl.h>
#include "wow_user_object_bindings.h"
#include "wow_cleanup_bindings.h"
#include <dde.h>
#include "wow_window_dispatch_bindings.h"
#include "wow_class_lookup_bindings.h"
#include "compat/include/wow_user_runtime.h"
#include "wow_task_order_bindings.h"
#include "wow_window_words_binding.h"
#include "compat/include/wow_page_domain.h"

/* Original query reads shared publication, never the cleanup ownership table.
 * One copied metadata row suffices; no native pointer, table-sized copy or
 * additional handle registry is required. The original body cannot reenter. */
typedef wow_cleanup_handle *PHE;
static const struct { ULONG cHandleEntries; } original_handle_limits = {0x10000u};
static __declspec(thread) PHE original_shared_entry;
#define gpsi (&original_handle_limits)
#define WowSharedHandleEntry(index) (original_shared_entry)
#define HMIndexFromHandle(handle) ((DWORD)(WORD)(handle))
#define TYPE_WINDOW (wow_cleanup_provider_current()->window_type)
#include "compat/include/wow_full_user_handle.h"
#undef TYPE_WINDOW
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
    result = GetFullUserHandle(handle);
    original_shared_entry = previous;
    return result;
}

static BOOL project_graph(const HWND *, ULONG, const HWND *, ULONG);

static BOOL refresh_graph(HWND callback_window, const HWND *payload, ULONG payload_count)
{
    wow_user_runtime_thread *binding;
    wow_user_object_table *objects;
    HWND *roots, desktop;
    ULONG count = 0, i;
    BOOL result;
    DWORD error, saved = GetLastError();
    if (payload_count && !payload) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
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
        if (objects->entries[i].bType != wow_cleanup_provider_current()->window_type || !window ||
                window == desktop || window == callback_window) continue;
        roots[count++] = window;
    }
    result = project_graph(roots, count, payload, payload_count);
    error = result ? saved : GetLastError();
    HeapFree(GetProcessHeap(), 0, roots);
    SetLastError(error);
    return result;
}

static BOOL refresh_graph_owned(HWND callback_window, const HWND *payload,
    ULONG count)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    BOOL held, result;
    DWORD error;
    /* An inactive domain has no shared graph; the existing publisher owns
     * validation of an active domain without a bound task. */
    if (!mvdm_softpc_wow_page_domain_active() || !binding)
        return refresh_graph(callback_window, payload, count);
    held = binding->exclusive_held;
    if (!held && !wow_user_runtime_enter(binding)) return FALSE;
    __try {
        result = refresh_graph(callback_window, payload, count);
    } __finally {
        error = GetLastError();
        if (!held && !wow_user_runtime_leave(binding))
            wow_user_native_call_failed("release-window-graph-owner", GetLastError());
        SetLastError(error);
    }
    return result;
}

BOOL WINAPI wow_user_window_refresh_graph(HWND callback_window)
{
    return refresh_graph_owned(callback_window, NULL, 0);
}

BOOL WINAPI wow_user_window_refresh_payload(HWND callback_window,
    const HWND *payload, ULONG count)
{
    return refresh_graph_owned(callback_window, payload, count);
}

typedef mvdm_softpc_wow_window_observation window_observation;

HWND WINAPI wow_user_window_native_handle(HWND window)
{
    wow_user_runtime_thread *binding;
    if (!window || ((ULONG)(ULONG_PTR)window != LOWORD(window) &&
            (LONG)(LONG_PTR)window != (LONG)(SHORT)LOWORD(window))) return window;
    binding = wow_user_runtime_current();
    if (!binding || !binding->thread || !binding->thread->ppi ||
            !binding->thread->ppi->objects) {
        SetLastError(ERROR_INVALID_STATE); return NULL;
    }
    return (HWND)(ULONG_PTR)wow_user_full_handle(
        binding->thread->ppi->objects, LOWORD(window));
}

HWND WINAPI wow_user_window_owned_handle(HWND window, BOOL allow_null)
{
    wow_user_runtime_thread *binding;
    HWND full;
    ULONG value;
    if (!window) {
        if (allow_null) return NULL;
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return NULL;
    }
    binding = wow_user_runtime_current();
    if (!binding || !binding->thread || !binding->thread->ppi ||
            !binding->thread->ppi->objects) {
        SetLastError(ERROR_INVALID_STATE);
        return NULL;
    }
    full = wow_user_window_full_handle(binding->thread->ppi->objects,
        LOWORD(window));
    if (!full) return NULL;
    value = (ULONG)(ULONG_PTR)window;
    if (value != LOWORD(window) &&
            (LONG)(LONG_PTR)window != (LONG)(SHORT)LOWORD(window) &&
            window != full) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    return full;
}

HWND WINAPI wow_user_window_public_handle(HWND window)
{
    wow_user_runtime_thread *binding;
    ULONG full;
    if (!window) return NULL;
    /* This is deliberately narrower than wow_user_window_native_handle().
     * A public modern USER boundary accepts only the original HWND32 carrier:
     * either zero-extended or sign-extended from a 16-bit guest HWND.  Letting
     * an arbitrary full native HWND pass through would hide a caller which
     * escaped the sole identity authority (the worker's published table). */
    if ((ULONG)(ULONG_PTR)window != LOWORD(window) &&
            (LONG)(LONG_PTR)window != (LONG)(SHORT)LOWORD(window)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    binding = wow_user_runtime_current();
    if (!binding || !binding->thread || !binding->thread->ppi ||
            !binding->thread->ppi->objects) {
        SetLastError(ERROR_INVALID_STATE); return NULL;
    }
    /* MVDM-HOST-DIV-454: only a public modern USER call needs this live
     * HWND; every original conversion continues to use native_handle above. */
    full = (ULONG)(ULONG_PTR)wow_user_window_full_handle(
        binding->thread->ppi->objects, LOWORD(window));
    if (full) return (HWND)(ULONG_PTR)full;
    /* A public Clipboard return can legitimately be an externally observed
     * window.  It was admitted only through refresh_payload(), which committed
     * this exact full HWND into the existing page-domain view.  Reuse that
     * same snapshot on the reverse public boundary so a returned viewer can
     * be supplied to the next original ChangeClipboardChain call.  This is
     * deliberately not used by the general original HWND conversion path. */
    full = mvdm_softpc_wow_page_domain_window_handle(LOWORD(window));
    return (HWND)(ULONG_PTR)full;
}

HWND WINAPI wow_user_window_public_guest_handle(HWND window)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    ULONG full;
    HWND guest;
    if (!binding || !binding->thread || !binding->thread->ppi ||
            !binding->thread->ppi->objects) return NULL;
    if (!window) return NULL;
    /* A public USER result can name another process's window.  The existing
     * graph transaction is the sole shared-view publisher: refresh this typed
     * result before lookup, then accept only its precise committed generation.
     * This reuses original-style observed HWND identity, rather than either
     * fabricating an alias or truncating the host's native value. */
    if (!wow_user_window_refresh_payload(NULL, &window, 1u)) return NULL;
    guest = wow_user_window_guest_handle(binding->thread->ppi->objects, window);
    if (guest) return guest;
    full = mvdm_softpc_wow_page_domain_window_handle(LOWORD(window));
    if (full == (ULONG)(ULONG_PTR)window)
        return (HWND)(ULONG_PTR)LOWORD(window);
    return NULL;
}

BOOL WINAPI wow_user_window_refresh_message(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    HWND roots[4];
    ULONG count = wow_user_message_window_roots(message, wp, lp, roots);
    if (window) {
        window = wow_user_window_native_handle(window);
        if (!window) return FALSE;
    }
    return wow_user_window_refresh_payload(window, roots, count);
}

static BOOL project_graph_attempt(const HWND *roots, ULONG root_count,
    const HWND *payload, ULONG payload_count, BOOL *capture_raced)
{
    window_observation *rows = NULL, captured;
    ULONG *index = NULL, count = 0, capacity = 0, i, used;
    BOOL result = FALSE;
    DWORD error, pid = GetCurrentProcessId(), saved = GetLastError();
    if (capture_raced) *capture_raced = FALSE;
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
    /* ADAPTER-WOW-094: these exact full identities are captured again below
     * before any validation/publication. Do not first capture a value that
     * will be overwritten. Reuse transaction scratch, not a retained cache. */
    for (i = 0; i < root_count; ++i)
        index[LOWORD(roots[i])] = (ULONG)(ULONG_PTR)roots[i];
    for (i = used = 0; i < count; ++i) {
        HWND window = (HWND)(ULONG_PTR)rows[i].links.window;
        if (index[LOWORD(window)] == (ULONG)(ULONG_PTR)window) {
            rows[used++] = rows[i];
            continue; /* Mandatory ordered capture below must still succeed. */
        }
        /* ADAPTER-WOW-082: route E already permits retained external
         * observations. This isn't another native desktop lock/freshness
         * gate. Local state stays synchronous; explicit roots/payloads below
         * still capture even when an external row is already represented. */
        if (rows[i].process_id && rows[i].process_id != pid &&
                rows[i].descriptor.atom) {
            if (!IsWindow(window)) continue;
        } else if (wow_native_window_capture(window, &captured)) rows[i] = captured;
        else if (!IsWindow(window)) {
            /* Withdraw expired observations with their incoming links. This
             * includes local native controls without a WW cleanup owner. */
            continue;
        } else if (rows[i].process_id == pid) {
            goto done; /* Live local state must not fall back to old fields. */
        } else if (!rows[i].descriptor.atom) goto done; /* Cold desktop. */
        rows[used] = rows[i];
        ++used;
    }
    count = used;
    /* Full-identity marks must never be consumed as observation offsets. */
    ZeroMemory(index, 0x10000u * sizeof(*index));
    for (i = 0; i < count; ++i)
        index[LOWORD(rows[i].links.window)] = i + 1u;
    for (i = 0; i < root_count; ++i) {
        if (!wow_native_window_capture(roots[i], &captured) ||
                !wow_native_window_observation_put(&rows, &count, &capacity, index, &captured)) goto done;
    }
    /* Typed secondary identities can be unlinked or disappear during a
     * native call. Sample live full identities in this same transaction;
     * never revive dead values or bind their low words to new generations.
     * Unlike a required receiver, a dead payload remains a numeric value
     * for the original converter and guest to interpret. */
    for (i = 0; i < payload_count; ++i) {
        HWND window;
        if (!payload[i]) continue;
        window = wow_user_window_native_handle(payload[i]);
        if (!window) goto done;
        if (wow_native_window_capture(window, &captured)) {
            if (!wow_native_window_observation_put(&rows, &count, &capacity, index, &captured)) goto done;
        } else if (IsWindow(window)) goto done;
    }
    for (i = 0; i < count; ++i) {
        unsigned edge;
        for (edge = 0; edge < 4u; ++edge) {
            ULONG target = *wow_native_window_observation_edge(&rows[i], edge), found;
            if (!target) continue;
            found = index[LOWORD(target)];
            if (found && rows[found - 1u].links.window == target) continue;
            if (wow_native_window_capture((HWND)(ULONG_PTR)target, &captured)) {
                if (!wow_native_window_observation_put(&rows, &count, &capacity, index, &captured)) goto done;
            } else {
                /* A vanished external endpoint is unavailable, not an alias
                 * for a reused slot. No native hierarchy is mutated here. */
                if (IsWindow((HWND)(ULONG_PTR)target)) {
                    DWORD owner = 0;
                    GetWindowThreadProcessId((HWND)(ULONG_PTR)target, &owner);
                    if (owner == pid) goto done;
                }
                *wow_native_window_observation_edge(&rows[i], edge) = 0;
            }
        }
    }
    /* A replacement generation may have displaced an earlier observation
     * during closure. Only observation edges may retain that old identity. */
    for (i = 0; i < count; ++i) {
        unsigned edge;
        for (edge = 0; edge < 4u; ++edge) {
            ULONG *target = wow_native_window_observation_edge(&rows[i], edge), found;
            if (!*target) continue;
            found = index[LOWORD(*target)];
            if (found && rows[found - 1u].links.window == *target) continue;
            if (rows[i].process_id == pid) { SetLastError(ERROR_INVALID_STATE); goto done; }
            *target = 0;
        }
    }
    if (!wow_native_window_normalize_chains(rows, count, index, capture_raced)) goto done;
    /* Prepare local names/references before the existing graph commit.
     * No fallible per-window refresh may follow that irreversible commit. */
    result = wow_window_words_project_observed(count, rows);
done:
    error = result ? saved : GetLastError();
    if (index) HeapFree(GetProcessHeap(), 0, index);
    if (rows) HeapFree(GetProcessHeap(), 0, rows);
    SetLastError(error);
    return result;
}

static BOOL project_graph(const HWND *roots, ULONG root_count,
    const HWND *payload, ULONG payload_count)
{
    BOOL capture_raced = FALSE;
    DWORD saved = GetLastError();
    if (project_graph_attempt(roots, root_count, payload, payload_count, &capture_raced))
        return TRUE;
    if (!capture_raced) return FALSE;
    /* ADAPTER-WOW-102: one fresh, fully validated acquisition, never a stale
     * fallback or retry of a failed commit/identity/native operation. */
    SetLastError(saved);
    return project_graph_attempt(roots, root_count, payload, payload_count, NULL);
}

BOOL WINAPI wow_user_window_project_graph(const HWND *roots, ULONG root_count)
{
    return project_graph(roots, root_count, NULL, 0);
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
    if (entry->bType!=wow_cleanup_provider_current()->free_type) {
        SetLastError(ERROR_ALREADY_EXISTS); return FALSE;
    }
    entry->phead=object; entry->pOwner=thread;
    entry->wUniq=HIWORD(window); entry->bFlags=0;
    entry->bType=wow_cleanup_provider_current()->window_type;
    if (!mvdm_softpc_wow_page_domain_publish_handle(index, entry->wUniq,
            entry->bType, entry->bFlags, (unsigned long)(ULONG_PTR)window,
            object->pcls ? object->pcls->guest_server : 0u, object->adwWOW)) {
        ZeroMemory(entry,sizeof(*entry));
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    table->windows[index]=window;
    object->native_window=window;
    if (index>table->last_handle) table->last_handle=index;
    return TRUE;
}

BOOL WINAPI wow_user_window_cancel_native_adoption(wow_user_object_table *table,
    HWND window, struct wow_cleanup_window *object, ULONG backing)
{
    wow_cleanup_handle *entry;
    if (!table || !object) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    entry = &table->entries[LOWORD(window)];
    if (entry->bType != wow_cleanup_provider_current()->window_type || entry->wUniq != HIWORD(window) ||
            entry->phead != object || table->windows[LOWORD(window)] != window) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    if (!mvdm_softpc_wow_page_domain_cancel_native_adoption(
            (ULONG)(ULONG_PTR)window, backing)) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    ZeroMemory(entry, sizeof(*entry));
    table->windows[LOWORD(window)] = NULL;
    return TRUE;
}

BOOL WINAPI wow_user_window_retire(wow_user_object_table *table, HWND window,
    struct wow_cleanup_window *object, ULONG *retained_backing)
{
    wow_cleanup_handle *entry;
    if (!table || !object) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    entry=&table->entries[LOWORD(window)];
    if (entry->bType!=wow_cleanup_provider_current()->window_type || entry->wUniq!=HIWORD(window) || entry->phead!=object) {
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
    if (entry->bType != wow_cleanup_provider_current()->window_type || !entry->phead ||
            LOWORD(table->windows[index]) != index) {
        SetLastError(ERROR_NOT_FOUND);
        return NULL;
    }
    return table->windows[index];
}

HWND WINAPI wow_user_window_guest_handle(wow_user_object_table *table,
    HWND window)
{
    WORD index;
    wow_cleanup_handle *entry;
    if (!table || !window) return NULL;
    /* The existing publication table is indexed by the original short USER
     * handle.  Verify the whole native identity before returning that index:
     * a foreign HWND with the same low word is not a guest handle. */
    index = LOWORD(window);
    if (!index || index == 0xffffu) return NULL;
    entry = &table->entries[index];
    if (entry->bType != wow_cleanup_provider_current()->window_type || !entry->phead ||
            table->windows[index] != window) return NULL;
    return (HWND)(ULONG_PTR)index;
}
