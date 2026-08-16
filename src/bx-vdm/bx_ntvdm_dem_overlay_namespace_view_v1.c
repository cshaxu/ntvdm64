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
