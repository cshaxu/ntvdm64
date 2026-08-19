#ifndef BX_NTVDM_COMMAND_PROFILE_CONSUMER_V1_H
#define BX_NTVDM_COMMAND_PROFILE_CONSUMER_V1_H

#include <stdint.h>
#include "bx_ntvdm_mutation_profile_v1.h"

#define BX_NTVDM_COMMAND_PROFILE_CONSUMER_V1_MAGIC 0x42584350u
#define BX_NTVDM_COMMAND_PROFILE_CONSUMER_V1_VERSION 1u

/* COMMAND consumes one copied, selector-blind policy record.  It contains
 * no host resource: later context/stream/process providers resolve their
 * capability class through this record before touching a host API. */
typedef struct bx_ntvdm_command_profile_consumer_v1 {
    uint32_t magic, abi_version, struct_bytes, reserved0;
    bx_ntvdm_mutation_profile_v1 profile;
} bx_ntvdm_command_profile_consumer_v1;

int bx_ntvdm_command_profile_consumer_v1_register_class(
    bx_ntvdm_mutation_profile_v1 *profile, uint32_t mutation_class,
    uint32_t permitted_mode_mask);
int bx_ntvdm_command_profile_consumer_v1_initialize(
    bx_ntvdm_command_profile_consumer_v1 *consumer,
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_command_profile_consumer_v1_valid(
    const bx_ntvdm_command_profile_consumer_v1 *consumer);
int bx_ntvdm_command_profile_consumer_v1_resolve(
    const bx_ntvdm_command_profile_consumer_v1 *consumer,
    uint32_t mutation_class, uint32_t *policy_result_out);

#endif
