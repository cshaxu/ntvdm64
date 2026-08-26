#ifndef RUNTIME_OPENNT_BOP_DEM_DIRECT_HOST_SESSION_H
#define RUNTIME_OPENNT_BOP_DEM_DIRECT_HOST_SESSION_H

#include "adapter-softpc/dem/direct_context.h"
#include "host_handle_manager.h"

#define RUNTIME_DEM_DIRECT_HOST_SESSION_MAGIC 0x42584448u
#define RUNTIME_DEM_DIRECT_HOST_SESSION_VERSION 1u

typedef struct runtime_dem_direct_host_session {
    uint32_t magic, abi_version, struct_bytes, bound;
    runtime_host_handle_manager *handles;
    runtime_dem_direct_context context;
} runtime_dem_direct_host_session;

int runtime_dem_direct_host_session_initialize(
    runtime_dem_direct_host_session *session);
void runtime_dem_direct_host_session_reset(
    runtime_dem_direct_host_session *session);
runtime_dem_direct_context *runtime_dem_direct_host_session_context(
    runtime_dem_direct_host_session *session);

/* These callbacks are intentionally exported only as a Direct DEM session
 * seam.  They transport checked ordinary RAM and no BOP/service identity. */
int runtime_dem_direct_host_session_guest_read(void *state,
    uint32_t address, uint8_t *bytes, uint32_t byte_count);
int runtime_dem_direct_host_session_guest_write(void *state,
    uint32_t address, const uint8_t *bytes, uint32_t byte_count);

#endif
