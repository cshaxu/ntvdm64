#include "bx_ntvdm_dem_overlay_namespace_view_v1.h"

#include <string.h>

static int prefix(const wchar_t *path, const wchar_t *candidate)
{
    size_t length;
    if (path == 0 || candidate == 0) return 0;
    length = wcslen(candidate);
    return _wcsnicmp(path, candidate, length) == 0 &&
        (path[length] == L'\0' || path[length] == L'\\');
}

/* A directory tombstone masks its visible subtree.  The check deliberately
 * uses the visible, not relocated effective path: an old source tombstone
 * must not hide a destination which resolves back to that source tree. */
static int directory_hidden(const bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive, const wchar_t *visible)
{
    uint32_t index;
    for (index = 0u; index < store->count; ++index) {
        const bx_ntvdm_dem_overlay_store_v1_entry *entry = &store->entries[index];
        if (entry->drive_index == drive &&
            entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY_TOMBSTONE &&
            prefix(visible, entry->relative)) return 1;
    }
    return 0;
}

/* A relocation is also the old-source mask.  Keeping that mask in the
 * relocation record rather than overwriting the source entry lets the visible
 * destination continue to inherit an unmaterialized base tree lazily. */
static int relocated_source_hidden(const bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive, const wchar_t *visible)
{
    uint32_t index;
    for (index = 0u; index < store->relocation_count; ++index) {
        const bx_ntvdm_dem_overlay_store_v1_relocation *entry =
            &store->relocations[index];
        if (entry->drive_index == drive && prefix(visible, entry->source)) return 1;
    }
    return 0;
}

static int from_entry(const bx_ntvdm_dem_overlay_store_v1_entry *entry,
    bx_ntvdm_dem_overlay_namespace_node_v1 *node)
{
    if (entry == 0) return 0;
    if (entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE) {
        node->kind = BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_FILE;
        node->attributes = entry->attributes;
        return 1;
    }
    if (entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY) {
        node->kind = BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY;
        node->attributes = entry->attributes | FILE_ATTRIBUTE_DIRECTORY;
        return 1;
    }
    return -1;
}

int bx_ntvdm_dem_overlay_namespace_view_v1_query(
    const bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive,
    const wchar_t *visible, bx_ntvdm_dem_overlay_namespace_node_v1 *node,
    DWORD *error_out)
{
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    wchar_t effective[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    DWORD file_error = ERROR_FILE_NOT_FOUND, directory_error = ERROR_PATH_NOT_FOUND;
    DWORD host_attributes = 0u;
    if (node != 0) memset(node, 0, sizeof(*node));
    if (error_out != 0) *error_out = ERROR_INVALID_PARAMETER;
    if (store == 0 || host_namespace == 0 || visible == 0 || node == 0 ||
        error_out == 0 || !bx_ntvdm_dem_overlay_store_v1_valid(store) ||
        !bx_ntvdm_host_namespace_v1_valid(host_namespace) || drive >= 26u ||
        visible[0] == L'\0') return 0;
    if (directory_hidden(store, drive, visible) ||
        relocated_source_hidden(store, drive, visible)) {
        *error_out = ERROR_FILE_NOT_FOUND;
        return 1;
    }
    entry = bx_ntvdm_dem_overlay_store_v1_lookup(store, drive, visible);
    if (entry != 0) {
        int value = from_entry(entry, node);
        *error_out = value > 0 ? ERROR_SUCCESS : ERROR_FILE_NOT_FOUND;
        return 1;
    }
    if (!bx_ntvdm_dem_overlay_store_v1_resolve_relocation(store, drive, visible,
            effective)) return 0;
    entry = bx_ntvdm_dem_overlay_store_v1_lookup(store, drive, effective);
    if (entry != 0) {
        int value = from_entry(entry, node);
        *error_out = value > 0 ? ERROR_SUCCESS : ERROR_FILE_NOT_FOUND;
        return 1;
    }
    if (bx_ntvdm_host_namespace_v1_query_file_attributes(host_namespace, drive,
            effective, &host_attributes, &file_error)) {
        node->kind = BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_FILE;
        node->attributes = host_attributes;
        *error_out = ERROR_SUCCESS;
        return 1;
    }
    if (bx_ntvdm_host_namespace_v1_directory_exists_ex(host_namespace, drive,
            effective, &directory_error)) {
        node->kind = BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY;
        node->attributes = FILE_ATTRIBUTE_DIRECTORY;
        *error_out = ERROR_SUCCESS;
        return 1;
    }
    *error_out = directory_error != ERROR_PATH_NOT_FOUND ? directory_error : file_error;
    return 1;
}

static int direct_child(const wchar_t *parent, const wchar_t *candidate)
{
    const wchar_t *suffix;
    if (parent != 0 && parent[0] == L'\0' && candidate != 0)
        return candidate[0] != L'\0' && wcschr(candidate, L'\\') == 0;
    if (!prefix(candidate, parent) || candidate[wcslen(parent)] != L'\\') return 0;
    suffix = candidate + wcslen(parent) + 1u;
    return suffix[0] != L'\0' && wcschr(suffix, L'\\') == 0;
}

static int visible_entry(const bx_ntvdm_dem_overlay_store_v1_entry *entry);

static int child_path(const wchar_t *parent, const wchar_t *name,
    wchar_t out[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE])
{
    size_t length;
    if (parent == 0 || name == 0 || out == 0) return 0;
    length = wcslen(parent);
    if (length + (length ? 1u : 0u) + wcslen(name) >= BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE)
        return 0;
    if (wcscpy_s(out, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, parent) != 0) return 0;
    if (length && wcscat_s(out, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, L"\\") != 0) return 0;
    return wcscat_s(out, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, name) == 0;
}

static int add_entry(bx_ntvdm_host_namespace_entry_v1 *entries, uint32_t capacity,
    uint32_t *count, const wchar_t *name, uint32_t attributes, uint64_t bytes,
    uint16_t time, uint16_t date)
{
    uint32_t index;
    if (entries == 0 || count == 0 || name == 0 || name[0] == L'\0') return 0;
    for (index = 0u; index < *count; ++index) if (_wcsicmp(entries[index].dos_name, name) == 0) {
        entries[index].attributes = attributes; entries[index].byte_count = bytes;
        entries[index].dos_time = time; entries[index].dos_date = date; return 1;
    }
    if (*count >= capacity || wcslen(name) >= 13u) return 0;
    memset(&entries[*count], 0, sizeof(entries[*count]));
    wcscpy_s(entries[*count].dos_name, 13u, name); entries[*count].attributes = attributes;
    entries[*count].byte_count = bytes; entries[*count].dos_time = time; entries[*count].dos_date = date;
    ++*count; return 1;
}

int bx_ntvdm_dem_overlay_namespace_view_v1_enumerate(
    const bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive,
    const wchar_t *visible, bx_ntvdm_host_namespace_entry_v1 *entries,
    uint32_t capacity, uint32_t *count_out, DWORD *error_out)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    bx_ntvdm_host_namespace_entry_v1 *base = 0;
    wchar_t effective[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    uint32_t count = 0u, base_count = 0u, index;
    DWORD error = ERROR_SUCCESS;
    if (count_out) *count_out = 0u; if (error_out) *error_out = ERROR_INVALID_PARAMETER;
    if (store == 0 || host_namespace == 0 || visible == 0 || entries == 0 || capacity == 0u ||
        capacity > BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES || count_out == 0 || error_out == 0 ||
        (!visible[0] && (host_namespace->available_mask & (UINT32_C(1) << drive)) == 0u) ||
        (visible[0] && (!bx_ntvdm_dem_overlay_namespace_view_v1_query(store, host_namespace,
            drive, visible, &node, &error) || node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY)) ||
        (visible[0] && !bx_ntvdm_dem_overlay_store_v1_resolve_relocation(store, drive, visible, effective))) {
        if (error_out) *error_out = error; return 0;
    }
    if (!visible[0]) effective[0] = L'\0';
    base = (bx_ntvdm_host_namespace_entry_v1 *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES * sizeof(*base));
    if (base == 0) { *error_out = ERROR_NOT_ENOUGH_MEMORY; return 0; }
    if (bx_ntvdm_host_namespace_v1_directory_exists_ex(host_namespace, drive, effective, &error) &&
        bx_ntvdm_host_namespace_v1_enumerate(host_namespace, drive, effective, base,
            BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES, &base_count) != BX_NTVDM_HOST_NAMESPACE_V1_OK) {
        HeapFree(GetProcessHeap(), 0u, base); *error_out = GetLastError(); return 0;
    }
    for (index = 0u; index < base_count; ++index) {
        wchar_t child[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
        if (!child_path(visible, base[index].dos_name, child) ||
            !bx_ntvdm_dem_overlay_namespace_view_v1_query(store, host_namespace, drive, child, &node, &error)) {
            HeapFree(GetProcessHeap(), 0u, base); *error_out = error; return 0;
        }
        if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT &&
            !add_entry(entries, capacity, &count, base[index].dos_name, node.attributes,
                base[index].byte_count, base[index].dos_time, base[index].dos_date)) {
            HeapFree(GetProcessHeap(), 0u, base); *error_out = ERROR_BUFFER_OVERFLOW; return 0;
        }
    }
    HeapFree(GetProcessHeap(), 0u, base);
    for (index = 0u; index < store->count; ++index) {
        const bx_ntvdm_dem_overlay_store_v1_entry *entry = &store->entries[index];
        const wchar_t *parent = direct_child(visible, entry->relative) ? visible : effective;
        const wchar_t *name;
        if (entry->drive_index != drive || !visible_entry(entry) || !direct_child(parent, entry->relative)) continue;
        name = entry->relative + (parent[0] == L'\0' ? 0u : wcslen(parent) + 1u);
        if (!add_entry(entries, capacity, &count, name, entry->attributes,
                entry->byte_count, entry->dos_time, entry->dos_date)) { *error_out = ERROR_BUFFER_OVERFLOW; return 0; }
    }
    for (index = 1u; index < count; ++index) {
        bx_ntvdm_host_namespace_entry_v1 item = entries[index];
        uint32_t prior = index;
        while (prior != 0u && _wcsicmp(entries[prior - 1u].dos_name, item.dos_name) > 0) {
            entries[prior] = entries[prior - 1u]; --prior;
        }
        entries[prior] = item;
    }
    *count_out = count; *error_out = ERROR_SUCCESS; return 1;
}

static int visible_entry(const bx_ntvdm_dem_overlay_store_v1_entry *entry)
{
    return entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE ||
        entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY;
}

int bx_ntvdm_dem_overlay_namespace_view_v1_directory_empty(
    const bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive,
    const wchar_t *visible, int *empty_out, DWORD *error_out)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    bx_ntvdm_host_namespace_entry_v1 *entries = 0;
    wchar_t effective[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    uint32_t count = 0u, index;
    DWORD error = ERROR_SUCCESS;
    int base_exists;
    if (empty_out != 0) *empty_out = 0;
    if (error_out != 0) *error_out = ERROR_INVALID_PARAMETER;
    if (empty_out == 0 || error_out == 0 || !bx_ntvdm_dem_overlay_namespace_view_v1_query(
            store, host_namespace, drive, visible, &node, &error)) return 0;
    if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) {
        *error_out = node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT ?
            error : ERROR_ACCESS_DENIED;
        return 1;
    }
    if (!bx_ntvdm_dem_overlay_store_v1_resolve_relocation(store, drive, visible,
            effective)) return 0;
    base_exists = bx_ntvdm_host_namespace_v1_directory_exists_ex(host_namespace,
        drive, effective, &error);
    if (base_exists) {
        entries = (bx_ntvdm_host_namespace_entry_v1 *)HeapAlloc(GetProcessHeap(),
            HEAP_ZERO_MEMORY, BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES * sizeof(*entries));
        if (entries == 0) { *error_out = ERROR_NOT_ENOUGH_MEMORY; return 1; }
        if (bx_ntvdm_host_namespace_v1_enumerate(host_namespace, drive, effective,
                entries, BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES, &count) !=
                BX_NTVDM_HOST_NAMESPACE_V1_OK) {
            HeapFree(GetProcessHeap(), 0u, entries);
            *error_out = GetLastError() == ERROR_SUCCESS ? ERROR_GEN_FAILURE : GetLastError();
            return 1;
        }
        for (index = 0u; index < count; ++index) {
            wchar_t child[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
            size_t length = wcslen(visible);
            if (length + 1u + wcslen(entries[index].dos_name) >=
                BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE) {
                HeapFree(GetProcessHeap(), 0u, entries); *error_out = ERROR_BUFFER_OVERFLOW; return 1;
            }
            memcpy(child, visible, length * sizeof(*child)); child[length++] = L'\\';
            wcscpy_s(child + length, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE - length,
                entries[index].dos_name);
            if (!bx_ntvdm_dem_overlay_namespace_view_v1_query(store, host_namespace,
                    drive, child, &node, &error)) {
                HeapFree(GetProcessHeap(), 0u, entries); return 0;
            }
            if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) {
                HeapFree(GetProcessHeap(), 0u, entries); *empty_out = 0; *error_out = ERROR_SUCCESS; return 1;
            }
        }
        HeapFree(GetProcessHeap(), 0u, entries);
    } else if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND) {
        *error_out = error;
        return 1;
    }
    for (index = 0u; index < store->count; ++index) {
        const bx_ntvdm_dem_overlay_store_v1_entry *entry = &store->entries[index];
        if (entry->drive_index == drive && visible_entry(entry) &&
            (direct_child(visible, entry->relative) ||
             direct_child(effective, entry->relative))) {
            *empty_out = 0; *error_out = ERROR_SUCCESS; return 1;
        }
    }
    *empty_out = 1; *error_out = ERROR_SUCCESS;
    return 1;
}
