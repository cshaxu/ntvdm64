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

int bx_ntvdm_dem_virtual_mutation_backend_v1_rename(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t source_drive,
    const wchar_t *source, uint8_t destination_drive, const wchar_t *destination,
    DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 source_node, destination_node;
    if (error != 0) *error = ERROR_INVALID_PARAMETER;
    if (store == 0 || source == 0 || destination == 0 || error == 0) return 0;
    if (source_drive != destination_drive) { *error = ERROR_NOT_SAME_DEVICE; return 1; }
    if (_wcsicmp(source, destination) == 0) { *error = ERROR_ACCESS_DENIED; return 1; }
    if (!bx_ntvdm_dem_virtual_namespace_view_v1_query(store, source_drive,
            source, &source_node, error)) return 0;
    if (source_node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 1;
    if (!bx_ntvdm_dem_virtual_namespace_view_v1_query(store, source_drive,
            destination, &destination_node, error)) return 0;
    if (destination_node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) {
        *error = ERROR_ALREADY_EXISTS; return 1;
    }
    if (!parent(store, source_drive, destination, error)) return 1;
    if (source_node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY &&
        _wcsnicmp(destination, source, wcslen(source)) == 0 &&
        destination[wcslen(source)] == L'\\') { *error = ERROR_ACCESS_DENIED; return 1; }
    if (!bx_ntvdm_dem_overlay_store_v1_move_private_subtree(store, source_drive,
            source, destination)) { *error = ERROR_NOT_ENOUGH_MEMORY; return 1; }
    *error = ERROR_SUCCESS; return 1;
}
