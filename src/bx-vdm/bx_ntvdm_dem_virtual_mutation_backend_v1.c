#include "bx_ntvdm_dem_virtual_mutation_backend_v1.h"

#include <string.h>

static int parent(const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *path, DWORD *error)
{
    wchar_t value[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    wchar_t *slash; bx_ntvdm_dem_overlay_namespace_node_v1 node;
    if (path == 0 || path[0] == L'\0' || wcscpy_s(value, 260u, path) != 0) return 0;
    slash = wcsrchr(value, L'\\');
    if (slash == 0) return 1;
    *slash = L'\0';
    if (!bx_ntvdm_dem_virtual_namespace_view_v1_query(store, drive, value, &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) return 1;
    *error = ERROR_PATH_NOT_FOUND; return 0;
}

int bx_ntvdm_dem_virtual_mutation_backend_v1_create_directory(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive, const wchar_t *path, DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    if (error) *error = ERROR_INVALID_PARAMETER;
    if (!store || !path || !error || !bx_ntvdm_dem_virtual_namespace_view_v1_query(store, drive, path, &node, error)) return 0;
    if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) { *error = ERROR_ALREADY_EXISTS; return 1; }
    if (!parent(store, drive, path, error)) return 1;
    if (!bx_ntvdm_dem_overlay_store_v1_put_directory(store, drive, path, FILE_ATTRIBUTE_DIRECTORY)) { *error = ERROR_NOT_ENOUGH_MEMORY; return 1; }
    *error = ERROR_SUCCESS; return 1;
}

int bx_ntvdm_dem_virtual_mutation_backend_v1_delete_file(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive, const wchar_t *path, DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    if (error) *error = ERROR_INVALID_PARAMETER;
    if (!store || !path || !error || !bx_ntvdm_dem_virtual_namespace_view_v1_query(store, drive, path, &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 1;
    if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_FILE) { *error = ERROR_ACCESS_DENIED; return 1; }
    if (!bx_ntvdm_dem_overlay_store_v1_tombstone(store, drive, path)) { *error = ERROR_NOT_ENOUGH_MEMORY; return 1; }
    *error = ERROR_SUCCESS; return 1;
}

int bx_ntvdm_dem_virtual_mutation_backend_v1_remove_directory(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive, const wchar_t *path, DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node; int empty;
    if (error) *error = ERROR_INVALID_PARAMETER;
    if (!store || !path || !error || !bx_ntvdm_dem_virtual_namespace_view_v1_query(store, drive, path, &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 1;
    if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) { *error = ERROR_PATH_NOT_FOUND; return 1; }
    if (!bx_ntvdm_dem_virtual_namespace_view_v1_directory_empty(store, drive, path, &empty, error)) return 0;
    if (!empty) { *error = ERROR_DIR_NOT_EMPTY; return 1; }
    if (!bx_ntvdm_dem_overlay_store_v1_tombstone_directory(store, drive, path)) { *error = ERROR_NOT_ENOUGH_MEMORY; return 1; }
    *error = ERROR_SUCCESS; return 1;
}
