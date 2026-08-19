#include "bx_ntvdm_dem_virtual_namespace_view_v1.h"

#include <string.h>

static int direct_child(const wchar_t *parent, const wchar_t *candidate)
{
    const wchar_t *suffix;
    if (parent == 0 || candidate == 0) return 0;
    if (parent[0] == L'\0') return candidate[0] != L'\0' && wcschr(candidate, L'\\') == 0;
    if (_wcsnicmp(candidate, parent, wcslen(parent)) != 0 ||
        candidate[wcslen(parent)] != L'\\') return 0;
    suffix = candidate + wcslen(parent) + 1u;
    return suffix[0] != L'\0' && wcschr(suffix, L'\\') == 0;
}

static int visible(const bx_ntvdm_dem_overlay_store_v1_entry *entry)
{
    return entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE ||
        entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY;
}

static int prefix(const wchar_t *path, const wchar_t *candidate)
{
    size_t length;
    if (path == 0 || candidate == 0) return 0;
    length = wcslen(candidate);
    return _wcsnicmp(path, candidate, length) == 0 &&
        (path[length] == L'\0' || path[length] == L'\\');
}

static int hidden(const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *relative)
{
    uint32_t index;
    for (index = 0u; index < store->count; ++index) {
        const bx_ntvdm_dem_overlay_store_v1_entry *entry = &store->entries[index];
        if (entry->drive_index == drive &&
            entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY_TOMBSTONE &&
            prefix(relative, entry->relative)) return 1;
    }
    return 0;
}

static int add(bx_ntvdm_host_namespace_entry_v1 *entries, uint32_t capacity,
    uint32_t *count, const bx_ntvdm_dem_overlay_store_v1_entry *entry,
    const wchar_t *name)
{
    uint32_t index;
    if (entries == 0 || count == 0 || entry == 0 || name == 0 || name[0] == L'\0' ||
        wcslen(name) >= 13u) return 0;
    for (index = 0u; index < *count; ++index) {
        if (_wcsicmp(entries[index].dos_name, name) == 0) return 0;
    }
    if (*count >= capacity) return 0;
    memset(&entries[*count], 0, sizeof(entries[*count]));
    wcscpy_s(entries[*count].dos_name, 13u, name);
    entries[*count].attributes = entry->attributes |
        (entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY ? FILE_ATTRIBUTE_DIRECTORY : 0u);
    entries[*count].byte_count = entry->byte_count;
    entries[*count].dos_time = entry->dos_time;
    entries[*count].dos_date = entry->dos_date;
    ++*count;
    return 1;
}

int bx_ntvdm_dem_virtual_namespace_view_v1_query(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *relative, bx_ntvdm_dem_overlay_namespace_node_v1 *node,
    DWORD *error_out)
{
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    if (node != 0) memset(node, 0, sizeof(*node));
    if (error_out != 0) *error_out = ERROR_INVALID_PARAMETER;
    if (store == 0 || relative == 0 || node == 0 || error_out == 0 || drive >= 26u ||
        !bx_ntvdm_dem_overlay_store_v1_valid(store)) return 0;
    if (relative[0] == L'\0') {
        node->kind = BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY;
        node->attributes = FILE_ATTRIBUTE_DIRECTORY;
        *error_out = ERROR_SUCCESS;
        return 1;
    }
    if (hidden(store, drive, relative)) { *error_out = ERROR_FILE_NOT_FOUND; return 1; }
    entry = bx_ntvdm_dem_overlay_store_v1_lookup(store, drive, relative);
    if (entry == 0 || !visible(entry)) { *error_out = ERROR_FILE_NOT_FOUND; return 1; }
    node->kind = entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY ?
        BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY : BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_FILE;
    node->attributes = entry->attributes |
        (node->kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY ? FILE_ATTRIBUTE_DIRECTORY : 0u);
    *error_out = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_dem_virtual_namespace_view_v1_enumerate(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *relative, bx_ntvdm_host_namespace_entry_v1 *entries,
    uint32_t capacity, uint32_t *count_out, DWORD *error_out)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    uint32_t count = 0u, index;
    if (count_out != 0) *count_out = 0u;
    if (error_out != 0) *error_out = ERROR_INVALID_PARAMETER;
    if (store == 0 || relative == 0 || entries == 0 || capacity == 0u || count_out == 0 ||
        error_out == 0 || capacity > BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES ||
        !bx_ntvdm_dem_virtual_namespace_view_v1_query(store, drive, relative, &node, error_out) ||
        node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) return 0;
    for (index = 0u; index < store->count; ++index) {
        const bx_ntvdm_dem_overlay_store_v1_entry *entry = &store->entries[index];
        const wchar_t *name;
        if (entry->drive_index != drive || !visible(entry) || hidden(store, drive, entry->relative) ||
            !direct_child(relative, entry->relative)) continue;
        name = entry->relative + (relative[0] == L'\0' ? 0u : wcslen(relative) + 1u);
        if (!add(entries, capacity, &count, entry, name)) { *error_out = ERROR_BUFFER_OVERFLOW; return 0; }
    }
    for (index = 1u; index < count; ++index) {
        bx_ntvdm_host_namespace_entry_v1 item = entries[index]; uint32_t prior = index;
        while (prior != 0u && _wcsicmp(entries[prior - 1u].dos_name, item.dos_name) > 0) {
            entries[prior] = entries[prior - 1u]; --prior;
        }
        entries[prior] = item;
    }
    *count_out = count; *error_out = ERROR_SUCCESS; return 1;
}

int bx_ntvdm_dem_virtual_namespace_view_v1_directory_empty(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *relative, int *empty_out, DWORD *error_out)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    uint32_t index;
    if (empty_out != 0) *empty_out = 0;
    if (error_out != 0) *error_out = ERROR_INVALID_PARAMETER;
    if (store == 0 || relative == 0 || empty_out == 0 || error_out == 0 ||
        !bx_ntvdm_dem_virtual_namespace_view_v1_query(store, drive, relative,
            &node, error_out)) return 0;
    if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) {
        if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT &&
            *error_out == ERROR_FILE_NOT_FOUND) *error_out = ERROR_PATH_NOT_FOUND;
        else *error_out = ERROR_ACCESS_DENIED;
        return 1;
    }
    for (index = 0u; index < store->count; ++index) {
        const bx_ntvdm_dem_overlay_store_v1_entry *entry = &store->entries[index];
        if (entry->drive_index == drive && visible(entry) && !hidden(store, drive, entry->relative) &&
            direct_child(relative, entry->relative)) { *error_out = ERROR_SUCCESS; return 1; }
    }
    *empty_out = 1; *error_out = ERROR_SUCCESS; return 1;
}
