#ifndef BX_NTVDM_DEM_FCB_OVERLAY_BACKEND_V1_H
#define BX_NTVDM_DEM_FCB_OVERLAY_BACKEND_V1_H

#include "bx_ntvdm_dem_overlay_namespace_backend_v1.h"
#include "bx_ntvdm_dem_overlay_handle_backend_v1.h"

/* Selector-blind FCB lifecycle backend.  Callers supply an already decoded
 * DOS path/open contract; this module never reads BOP bytes or guest memory. */
int bx_ntvdm_dem_fcb_overlay_backend_v1_open(
    bx_ntvdm_dem_file_session_v1 *session, bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *relative, uint32_t access, uint32_t share_access,
    DWORD creation_disposition, uint32_t attributes, uint32_t *token_out,
    uint32_t *size_out, uint16_t *time_out, uint16_t *date_out,
    DWORD *error_out);
int bx_ntvdm_dem_fcb_overlay_backend_v1_info(
    bx_ntvdm_dem_file_session_v1 *session, bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token, uint32_t *attributes_out, uint32_t *size_out,
    uint16_t *time_out, uint16_t *date_out);

#endif
