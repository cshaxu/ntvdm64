#ifndef BX_NTVDM_STARTUP_CONFIGURATION_POLICY_V1_H
#define BX_NTVDM_STARTUP_CONFIGURATION_POLICY_V1_H

#include <stdint.h>

#include "bx_ntvdm_mutation_profile_v1.h"

#define BX_NTVDM_STARTUP_CONFIGURATION_POLICY_V1_MAGIC 0x42584350u
#define BX_NTVDM_STARTUP_CONFIGURATION_POLICY_V1_VERSION 1u

/* This selects only the provenance category.  It deliberately carries no
 * pathname, image bytes, host handle, selector, or guest address. */
enum bx_ntvdm_startup_configuration_source_v1 {
    BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_CONTAINED_FIXTURE = 1u,
    BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_EXPLICIT_HOST = 2u,
    BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_VIRTUAL_VOLUME = 3u
};

typedef struct bx_ntvdm_startup_configuration_policy_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t source;
    uint32_t mutation_mode;
    uint32_t permitted_mode_mask;
    uint32_t reserved0;
} bx_ntvdm_startup_configuration_policy_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_startup_configuration_policy_v1_clear(
    bx_ntvdm_startup_configuration_policy_v1 *policy);
int bx_ntvdm_startup_configuration_policy_v1_initialize(
    bx_ntvdm_startup_configuration_policy_v1 *policy,
    const bx_ntvdm_mutation_profile_v1 *profile, uint32_t source);
int bx_ntvdm_startup_configuration_policy_v1_valid(
    const bx_ntvdm_startup_configuration_policy_v1 *policy);

#ifdef __cplusplus
}
#endif

#endif
