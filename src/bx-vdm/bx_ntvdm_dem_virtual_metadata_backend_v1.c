#include "bx_ntvdm_dem_virtual_metadata_backend_v1.h"

int bx_ntvdm_dem_virtual_metadata_backend_v1_query(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *path, uint32_t *attributes, DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    if (attributes != 0) *attributes = 0u;
    if (error != 0) *error = ERROR_INVALID_PARAMETER;
    if (attributes == 0 || error == 0 || !bx_ntvdm_dem_virtual_namespace_view_v1_query(
            store, drive, path, &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 1;
    *attributes = node.attributes; *error = ERROR_SUCCESS; return 1;
}

int bx_ntvdm_dem_virtual_metadata_backend_v1_set(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *path, uint32_t attributes, DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    if (error != 0) *error = ERROR_INVALID_PARAMETER;
    if (store == 0 || path == 0 || error == 0 ||
        !bx_ntvdm_dem_virtual_namespace_view_v1_query(store, drive, path, &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 1;
    if (!bx_ntvdm_dem_overlay_store_v1_set_attributes(store, drive, path, attributes)) {
        *error = ERROR_NOT_ENOUGH_MEMORY; return 1;
    }
    *error = ERROR_SUCCESS; return 1;
}

int bx_ntvdm_dem_virtual_metadata_backend_v1_check_directory(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *path, DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    if (error != 0) *error = ERROR_INVALID_PARAMETER;
    if (error == 0 || !bx_ntvdm_dem_virtual_namespace_view_v1_query(store,
            drive, path, &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) {
        *error = ERROR_SUCCESS; return 1;
    }
    if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT)
        *error = ERROR_PATH_NOT_FOUND;
    return 1;
}
