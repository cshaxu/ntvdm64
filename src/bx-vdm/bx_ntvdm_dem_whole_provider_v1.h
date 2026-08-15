#ifndef BX_NTVDM_DEM_WHOLE_PROVIDER_V1_H
#define BX_NTVDM_DEM_WHOLE_PROVIDER_V1_H

#include "bx_ntvdm_dem_local_file_backend_v1.h"

#define BX_NTVDM_DEM_WHOLE_PROVIDER_V1_MAGIC 0x42585750u
#define BX_NTVDM_DEM_WHOLE_PROVIDER_V1_VERSION 1u

/* This is the single state owner for T202/S4's 29 local-file identities.
 * It is deliberately not a BOP ingress or CPU/guest-memory interface. */
typedef struct bx_ntvdm_dem_whole_provider_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t reserved0;
    const bx_ntvdm_host_namespace_v1 *host_namespace;
    const bx_ntvdm_dem_cwd_context_v1 *cwd;
    bx_ntvdm_dem_file_session_v1 files;
    bx_ntvdm_dem_local_file_backend_v1 local_files;
} bx_ntvdm_dem_whole_provider_v1;

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_dem_whole_provider_v1_initialize(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_mutation_profile_v1 *profile,
    const bx_ntvdm_host_namespace_v1 *host_namespace,
    const bx_ntvdm_dem_cwd_context_v1 *cwd);
int bx_ntvdm_dem_whole_provider_v1_valid(
    const bx_ntvdm_dem_whole_provider_v1 *provider);
void bx_ntvdm_dem_whole_provider_v1_teardown(
    bx_ntvdm_dem_whole_provider_v1 *provider);

/* Membership is an owner-package guard. It does not imply implementation of
 * the service or permit a selector-specific profile decision. */
int bx_ntvdm_dem_whole_provider_v1_owns_service(uint8_t service);

#ifdef __cplusplus
}
#endif

#endif
