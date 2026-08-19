#include "bx_ntvdm_dem_overlay_mutation_backend_v1.h"

#include <string.h>

static int parent_exists(const bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host, uint8_t drive, const wchar_t *path,
    DWORD *error)
{
    wchar_t parent[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    wchar_t *slash;
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    wcscpy_s(parent, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, path);
    slash = wcsrchr(parent, L'\\');
    if (slash == 0) {
        if ((host->available_mask & (UINT32_C(1) << drive)) == 0u) {
            *error = ERROR_PATH_NOT_FOUND; return 0;
        }
        return 1;
    }
    *slash = L'\0';
    if (!bx_ntvdm_dem_overlay_namespace_view_v1_query(store, host, drive,
            parent, &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) return 1;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT &&
        (*error == ERROR_SUCCESS || *error == ERROR_FILE_NOT_FOUND))
        *error = ERROR_PATH_NOT_FOUND;
    else if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY)
        *error = ERROR_PATH_NOT_FOUND;
    return 0;
}

static int absent(const bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host, uint8_t drive, const wchar_t *path,
    DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    if (!bx_ntvdm_dem_overlay_namespace_view_v1_query(store, host, drive, path,
            &node, error)) return 0;
    return node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT;
}

static int effective_for_visible(const bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive, const wchar_t *visible,
    wchar_t effective[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE])
{
    const bx_ntvdm_dem_overlay_store_v1_entry *entry =
        bx_ntvdm_dem_overlay_store_v1_lookup(store, drive, visible);
    if (entry != 0 && (entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE ||
        entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY)) {
        return wcscpy_s(effective, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, visible) == 0;
    }
    return bx_ntvdm_dem_overlay_store_v1_resolve_relocation(store, drive, visible,
        effective);
}

int bx_ntvdm_dem_overlay_mutation_backend_v1_create_directory(
    bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host, uint8_t drive, const wchar_t *path,
    DWORD *error)
{
    if (error != 0) *error = ERROR_INVALID_PARAMETER;
    if (store == 0 || host == 0 || path == 0 || error == 0 ||
        !bx_ntvdm_dem_overlay_store_v1_valid(store) ||
        !bx_ntvdm_host_namespace_v1_valid(host) || drive >= 26u) return 0;
    if (!absent(store, host, drive, path, error)) {
        if (*error == ERROR_SUCCESS) *error = ERROR_ALREADY_EXISTS;
        return 1;
    }
    if (!parent_exists(store, host, drive, path, error)) return 1;
    if (!bx_ntvdm_dem_overlay_store_v1_put_directory(store, drive, path,
            FILE_ATTRIBUTE_DIRECTORY)) { *error = ERROR_NOT_ENOUGH_MEMORY; return 1; }
    *error = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_dem_overlay_mutation_backend_v1_delete_file(
    bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host, uint8_t drive, const wchar_t *path,
    DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    if (error != 0) *error = ERROR_INVALID_PARAMETER;
    if (store == 0 || host == 0 || path == 0 || error == 0 ||
        !bx_ntvdm_dem_overlay_namespace_view_v1_query(store, host, drive, path,
            &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 1;
    if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_FILE) {
        *error = ERROR_ACCESS_DENIED; return 1;
    }
    if (!bx_ntvdm_dem_overlay_store_v1_tombstone(store, drive, path)) {
        *error = ERROR_NOT_ENOUGH_MEMORY; return 1;
    }
    *error = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_dem_overlay_mutation_backend_v1_remove_directory(
    bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host, uint8_t drive, const wchar_t *path,
    DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    int empty;
    if (error != 0) *error = ERROR_INVALID_PARAMETER;
    if (store == 0 || host == 0 || path == 0 || error == 0 ||
        !bx_ntvdm_dem_overlay_namespace_view_v1_query(store, host, drive, path,
            &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 1;
    if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) {
        *error = ERROR_PATH_NOT_FOUND; return 1;
    }
    if (!bx_ntvdm_dem_overlay_namespace_view_v1_directory_empty(store, host,
            drive, path, &empty, error)) return 0;
    if (*error != ERROR_SUCCESS || !empty) {
        if (*error == ERROR_SUCCESS) *error = ERROR_DIR_NOT_EMPTY;
        return 1;
    }
    if (!bx_ntvdm_dem_overlay_store_v1_tombstone_directory(store, drive, path)) {
        *error = ERROR_NOT_ENOUGH_MEMORY; return 1;
    }
    *error = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_dem_overlay_mutation_backend_v1_rename(
    bx_ntvdm_dem_overlay_store_v1 *store, bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *host, uint8_t source_drive,
    const wchar_t *source, uint8_t destination_drive, const wchar_t *destination,
    DWORD *error)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    wchar_t effective[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    uint32_t token, size;
    if (error != 0) *error = ERROR_INVALID_PARAMETER;
    if (store == 0 || files == 0 || host == 0 || source == 0 || destination == 0 ||
        error == 0 || source_drive != destination_drive) {
        if (error != 0 && source_drive != destination_drive) *error = ERROR_NOT_SAME_DEVICE;
        return 0;
    }
    if (_wcsicmp(source, destination) == 0) { *error = ERROR_ACCESS_DENIED; return 1; }
    if (!bx_ntvdm_dem_overlay_namespace_view_v1_query(store, host, source_drive,
            source, &node, error)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 1;
    if (!absent(store, host, source_drive, destination, error)) {
        if (*error == ERROR_SUCCESS) *error = ERROR_ALREADY_EXISTS;
        return 1;
    }
    if (!parent_exists(store, host, source_drive, destination, error) ||
        !effective_for_visible(store, source_drive, source, effective)) return 0;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) {
        if (!bx_ntvdm_dem_overlay_store_v1_add_relocation(store, source_drive,
                destination, effective)) { *error = ERROR_NOT_ENOUGH_MEMORY; return 1; }
        if (_wcsicmp(source, effective) != 0 &&
            !bx_ntvdm_dem_overlay_store_v1_tombstone_directory(store, source_drive, source)) {
            *error = ERROR_NOT_ENOUGH_MEMORY; return 1;
        }
        *error = ERROR_SUCCESS;
        return 1;
    }
    if (node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_FILE ||
        !bx_ntvdm_dem_overlay_resolver_v1_open(files, host, source_drive, effective,
            BX_NTVDM_DEM_OVERLAY_FILE_V1_READ, OPEN_EXISTING, &token, &size, error)) return 1;
    if (!bx_ntvdm_dem_overlay_file_v1_close(files, token) ||
        !bx_ntvdm_dem_overlay_store_v1_copy_file(store, source_drive, effective,
            destination) || !bx_ntvdm_dem_overlay_store_v1_tombstone(store,
                source_drive, source)) { *error = ERROR_NOT_ENOUGH_MEMORY; return 1; }
    *error = ERROR_SUCCESS;
    return 1;
}
