#ifndef BX_NTVDM_BOP_SHIM_DEM_DIRECT_HOST_SESSION_H
#define BX_NTVDM_BOP_SHIM_DEM_DIRECT_HOST_SESSION_H

#include "bx_ntvdm_dem_direct_context.h"

#define BX_NTVDM_DEM_DIRECT_HOST_SESSION_MAGIC 0x42584448u
#define BX_NTVDM_DEM_DIRECT_HOST_SESSION_VERSION 1u
#define BX_NTVDM_DEM_DIRECT_HOST_SESSION_HANDLES 64u

typedef struct bx_ntvdm_dem_direct_host_session {
    uint32_t magic, abi_version, struct_bytes, bound;
    HANDLE handles[BX_NTVDM_DEM_DIRECT_HOST_SESSION_HANDLES];
    bx_ntvdm_dem_direct_context context;
} bx_ntvdm_dem_direct_host_session;

int bx_ntvdm_dem_direct_host_session_initialize(
    bx_ntvdm_dem_direct_host_session *session);
void bx_ntvdm_dem_direct_host_session_reset(
    bx_ntvdm_dem_direct_host_session *session);
bx_ntvdm_dem_direct_context *bx_ntvdm_dem_direct_host_session_context(
    bx_ntvdm_dem_direct_host_session *session);

/* These callbacks are intentionally exported only as a Direct DEM session
 * seam.  They transport checked ordinary RAM and no BOP/service identity. */
int bx_ntvdm_dem_direct_host_session_guest_read(void *state,
    uint32_t address, uint8_t *bytes, uint32_t byte_count);
int bx_ntvdm_dem_direct_host_session_guest_write(void *state,
    uint32_t address, const uint8_t *bytes, uint32_t byte_count);

#endif
