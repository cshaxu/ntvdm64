#ifndef BX_NTVDM_DEM_OVERLAY_NAMESPACE_BACKEND_V1_H
#define BX_NTVDM_DEM_OVERLAY_NAMESPACE_BACKEND_V1_H

#include "bx_ntvdm_dem_overlay_resolver_v1.h"
#include "bx_ntvdm_dem_file_session_v1.h"

/* Selector-blind demOpen/demCreate composition: a resolved DOS path becomes
 * a typed private COW token; neither a BOP nor a host handle enters here. */
int bx_ntvdm_dem_overlay_namespace_backend_v1_open(
    bx_ntvdm_dem_file_session_v1 *session, bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *relative, uint32_t access, uint32_t share_access,
    DWORD creation_disposition, uint16_t pdb_owner, uint32_t *token_out,
    uint32_t *size_out, DWORD *error_out);

#endif
