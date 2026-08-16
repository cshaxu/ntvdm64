#ifndef BX_NTVDM_DEM_OVERLAY_MUTATION_BACKEND_V1_H
#define BX_NTVDM_DEM_OVERLAY_MUTATION_BACKEND_V1_H

#include "bx_ntvdm_dem_overlay_namespace_view_v1.h"
#include "bx_ntvdm_dem_overlay_resolver_v1.h"

/* Source-derived, selector-blind namespace mutation transaction. Every
 * operation changes only the session-private Overlay store; host-root access
 * is restricted to the resolver's read-only base materialization. */
int bx_ntvdm_dem_overlay_mutation_backend_v1_create_directory(
    bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *relative, DWORD *error_out);
int bx_ntvdm_dem_overlay_mutation_backend_v1_delete_file(
    bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *relative, DWORD *error_out);
int bx_ntvdm_dem_overlay_mutation_backend_v1_remove_directory(
    bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *relative, DWORD *error_out);
int bx_ntvdm_dem_overlay_mutation_backend_v1_rename(
    bx_ntvdm_dem_overlay_store_v1 *store, bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t source_drive,
    const wchar_t *source, uint8_t destination_drive, const wchar_t *destination,
    DWORD *error_out);

#endif
