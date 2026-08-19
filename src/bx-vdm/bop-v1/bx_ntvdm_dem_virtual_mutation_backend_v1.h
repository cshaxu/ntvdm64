#ifndef BX_NTVDM_DEM_VIRTUAL_MUTATION_BACKEND_V1_H
#define BX_NTVDM_DEM_VIRTUAL_MUTATION_BACKEND_V1_H

#include "bx_ntvdm_dem_virtual_namespace_view_v1.h"

/* Selector-blind private namespace mutations.  There is intentionally no
 * host namespace, host path, or host handle input. */
int bx_ntvdm_dem_virtual_mutation_backend_v1_create_directory(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative, DWORD *error_out);
int bx_ntvdm_dem_virtual_mutation_backend_v1_delete_file(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative, DWORD *error_out);
int bx_ntvdm_dem_virtual_mutation_backend_v1_remove_directory(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative, DWORD *error_out);
int bx_ntvdm_dem_virtual_mutation_backend_v1_rename(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t source_drive_index,
    const wchar_t *source_relative, uint8_t destination_drive_index,
    const wchar_t *destination_relative, DWORD *error_out);

#endif
