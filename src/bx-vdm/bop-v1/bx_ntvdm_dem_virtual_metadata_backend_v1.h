#ifndef BX_NTVDM_DEM_VIRTUAL_METADATA_BACKEND_V1_H
#define BX_NTVDM_DEM_VIRTUAL_METADATA_BACKEND_V1_H

#include "bx_ntvdm_dem_virtual_namespace_view_v1.h"

/* Private-root metadata companion to the Virtual namespace backend. */
int bx_ntvdm_dem_virtual_metadata_backend_v1_query(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative, uint32_t *attributes_out, DWORD *error_out);
int bx_ntvdm_dem_virtual_metadata_backend_v1_set(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative, uint32_t attributes, DWORD *error_out);
int bx_ntvdm_dem_virtual_metadata_backend_v1_check_directory(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative, DWORD *error_out);

#endif
