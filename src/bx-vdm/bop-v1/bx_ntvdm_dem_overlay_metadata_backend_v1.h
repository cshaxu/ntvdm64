#ifndef BX_NTVDM_DEM_OVERLAY_METADATA_BACKEND_V1_H
#define BX_NTVDM_DEM_OVERLAY_METADATA_BACKEND_V1_H

#include "bx_ntvdm_dem_overlay_namespace_view_v1.h"
#include "bx_ntvdm_dem_overlay_resolver_v1.h"

int bx_ntvdm_dem_overlay_metadata_backend_v1_query(
    const bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *relative, uint32_t *attributes_out, DWORD *error_out);
int bx_ntvdm_dem_overlay_metadata_backend_v1_set(
    bx_ntvdm_dem_overlay_store_v1 *store, bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *relative, uint32_t attributes, DWORD *error_out);
int bx_ntvdm_dem_overlay_metadata_backend_v1_check_directory(
    const bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *relative, DWORD *error_out);

#endif
