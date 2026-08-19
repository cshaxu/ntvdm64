#ifndef BX_NTVDM_DEM_PROFILE_CONSUMER_V1_H
#define BX_NTVDM_DEM_PROFILE_CONSUMER_V1_H

#include <stdint.h>

#include "bx_ntvdm_mutation_profile_v1.h"

#define BX_NTVDM_DEM_PROFILE_CONSUMER_V1_MAGIC 0x42584450u
#define BX_NTVDM_DEM_PROFILE_CONSUMER_V1_VERSION 1u

/* DEM's package-level consumption of the common mutation profile. This is
 * intentionally selector/service blind: leaves ask their package owner for a
 * class policy; the owner does not let them select a mode. */
typedef struct bx_ntvdm_dem_profile_consumer_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t reserved0;
    bx_ntvdm_mutation_profile_v1 profile;
} bx_ntvdm_dem_profile_consumer_v1;

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_dem_profile_consumer_v1_register_class(
    bx_ntvdm_mutation_profile_v1 *profile, uint32_t mutation_class,
    uint32_t permitted_mode_mask);
int bx_ntvdm_dem_profile_consumer_v1_initialize(
    bx_ntvdm_dem_profile_consumer_v1 *consumer,
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_dem_profile_consumer_v1_valid(
    const bx_ntvdm_dem_profile_consumer_v1 *consumer);
int bx_ntvdm_dem_profile_consumer_v1_resolve(
    const bx_ntvdm_dem_profile_consumer_v1 *consumer,
    uint32_t mutation_class, uint32_t *policy_result_out);

#ifdef __cplusplus
}
#endif

#endif
