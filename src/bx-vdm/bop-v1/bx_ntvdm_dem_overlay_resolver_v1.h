#ifndef BX_NTVDM_DEM_OVERLAY_RESOLVER_V1_H
#define BX_NTVDM_DEM_OVERLAY_RESOLVER_V1_H

#include "bx_ntvdm_dem_overlay_file_v1.h"
#include "bx_ntvdm_host_namespace.h"

/* Selector-blind bridge from an admitted, read-only host base to the DEM
 * provider's private COW file object.  It never opens a host file for write. */
int bx_ntvdm_dem_overlay_resolver_v1_open(
    bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *host_namespace,
    uint8_t drive_index, const wchar_t *relative, uint32_t access,
    DWORD creation_disposition, uint32_t *backend_token_out,
    uint32_t *size_out, DWORD *win32_error_out);
int bx_ntvdm_dem_overlay_resolver_v1_open_shared(
    bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *host_namespace,
    uint8_t drive_index, const wchar_t *relative, uint32_t access,
    uint32_t share_access, DWORD creation_disposition,
    uint32_t *backend_token_out, uint32_t *size_out, DWORD *win32_error_out);

#endif
