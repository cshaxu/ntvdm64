#ifndef BX_NTVDM_DEM_VIRTUAL_NAMESPACE_BACKEND_V1_H
#define BX_NTVDM_DEM_VIRTUAL_NAMESPACE_BACKEND_V1_H

#include "bx_ntvdm_dem_virtual_namespace_view_v1.h"
#include "bx_ntvdm_dem_overlay_file_v1.h"
#include "bx_ntvdm_dem_file_session_v1.h"

/* Selector-blind private-root open/create. It has no host namespace or
 * resolver input and returns only the existing opaque session token. */
int bx_ntvdm_dem_virtual_namespace_backend_v1_open(
    bx_ntvdm_dem_file_session_v1 *session, bx_ntvdm_dem_overlay_file_v1 *files,
    uint8_t drive_index, const wchar_t *relative, uint32_t access,
    uint32_t share_access, DWORD creation_disposition, uint32_t attributes,
    uint16_t pdb_owner, uint32_t *token_out, uint32_t *size_out,
    DWORD *error_out);

#endif
