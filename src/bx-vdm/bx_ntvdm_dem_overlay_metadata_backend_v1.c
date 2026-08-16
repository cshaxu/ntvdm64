#include "bx_ntvdm_dem_overlay_metadata_backend_v1.h"

static int effective(const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *visible, wchar_t out[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE])
{
    const bx_ntvdm_dem_overlay_store_v1_entry *entry =
        bx_ntvdm_dem_overlay_store_v1_lookup(store, drive, visible);
    if (entry != 0 && (entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE ||
        entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY))
        return wcscpy_s(out, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, visible) == 0;
    return bx_ntvdm_dem_overlay_store_v1_resolve_relocation(store, drive, visible, out);
}

int bx_ntvdm_dem_overlay_metadata_backend_v1_query(
    const bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host, uint8_t drive, const wchar_t *path,
    uint32_t *attributes, DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    if (attributes != 0) *attributes = 0u;
    if (error != 0) *error = ERROR_INVALID_PARAMETER;
    if (attributes == 0 || error == 0 || !bx_ntvdm_dem_overlay_namespace_view_v1_query(
            store, host, drive, path, &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 1;
    *attributes = node.attributes;
    *error = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_dem_overlay_metadata_backend_v1_set(
    bx_ntvdm_dem_overlay_store_v1 *store, bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *host, uint8_t drive, const wchar_t *path,
    uint32_t attributes, DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    wchar_t source[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    uint32_t token, size;
    if (error != 0) *error = ERROR_INVALID_PARAMETER;
    if (store == 0 || files == 0 || host == 0 || path == 0 || error == 0 ||
        !bx_ntvdm_dem_overlay_namespace_view_v1_query(store, host, drive, path,
            &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 1;
    if (!effective(store, drive, path, source)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) {
        if (bx_ntvdm_dem_overlay_store_v1_lookup(store, drive, path) == 0 &&
            !bx_ntvdm_dem_overlay_store_v1_put_directory(store, drive, path, attributes)) {
            *error = ERROR_NOT_ENOUGH_MEMORY; return 1;
        }
    } else if (bx_ntvdm_dem_overlay_store_v1_lookup(store, drive, path) == 0) {
        if (!bx_ntvdm_dem_overlay_resolver_v1_open(files, host, drive, source,
                BX_NTVDM_DEM_OVERLAY_FILE_V1_READ, OPEN_EXISTING, &token, &size, error) ||
            !bx_ntvdm_dem_overlay_file_v1_close(files, token) ||
            (_wcsicmp(source, path) != 0 && !bx_ntvdm_dem_overlay_store_v1_copy_file(
                store, drive, source, path))) {
            if (*error == ERROR_SUCCESS) *error = ERROR_NOT_ENOUGH_MEMORY;
            return 1;
        }
    }
    if (!bx_ntvdm_dem_overlay_store_v1_set_attributes(store, drive, path, attributes)) {
        *error = ERROR_NOT_ENOUGH_MEMORY; return 1;
    }
    *error = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_dem_overlay_metadata_backend_v1_check_directory(
    const bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host, uint8_t drive, const wchar_t *path,
    DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    if (error != 0) *error = ERROR_INVALID_PARAMETER;
    if (error == 0 || !bx_ntvdm_dem_overlay_namespace_view_v1_query(store, host,
            drive, path, &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) {
        *error = ERROR_SUCCESS; return 1;
    }
    if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT)
        *error = ERROR_PATH_NOT_FOUND;
    return 1;
}
