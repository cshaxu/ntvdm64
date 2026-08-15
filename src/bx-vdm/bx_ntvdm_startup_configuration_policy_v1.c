#include "bx_ntvdm_startup_configuration_policy_v1.h"

#include <string.h>

static uint32_t mode_bit(uint32_t mode)
{
    return mode >= BX_NTVDM_MUTATION_MODE_V1_DIRECT &&
        mode <= BX_NTVDM_MUTATION_MODE_V1_VIRTUAL ? 1u << (mode - 1u) : 0u;
}

static uint32_t source_mask(uint32_t source)
{
    switch (source) {
    case BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_CONTAINED_FIXTURE:
    case BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_EXPLICIT_HOST:
        return 0x0fu;
    case BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_VIRTUAL_VOLUME:
        return 1u << (BX_NTVDM_MUTATION_MODE_V1_VIRTUAL - 1u);
    default:
        return 0u;
    }
}

void bx_ntvdm_startup_configuration_policy_v1_clear(
    bx_ntvdm_startup_configuration_policy_v1 *policy)
{
    if (policy != 0) memset(policy, 0, sizeof(*policy));
}

int bx_ntvdm_startup_configuration_policy_v1_valid(
    const bx_ntvdm_startup_configuration_policy_v1 *policy)
{
    uint32_t expected_mask;
    if (policy == 0 ||
        policy->magic != BX_NTVDM_STARTUP_CONFIGURATION_POLICY_V1_MAGIC ||
        policy->abi_version != BX_NTVDM_STARTUP_CONFIGURATION_POLICY_V1_VERSION ||
        policy->struct_bytes != sizeof(*policy) || policy->reserved0 != 0u)
        return 0;
    expected_mask = source_mask(policy->source);
    return expected_mask != 0u && policy->permitted_mode_mask == expected_mask &&
        (expected_mask & mode_bit(policy->mutation_mode)) != 0u;
}

int bx_ntvdm_startup_configuration_policy_v1_initialize(
    bx_ntvdm_startup_configuration_policy_v1 *policy,
    const bx_ntvdm_mutation_profile_v1 *profile, uint32_t source)
{
    uint32_t mode;
    uint32_t permitted_mask = source_mask(source);
    if (policy == 0) return 0;
    bx_ntvdm_startup_configuration_policy_v1_clear(policy);
    if (permitted_mask == 0u ||
        !bx_ntvdm_mutation_profile_v1_authorize(profile,
            BX_NTVDM_MUTATION_OWNER_V1_COMMAND,
            BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, &mode) ||
        (permitted_mask & mode_bit(mode)) == 0u)
        return 0;
    policy->magic = BX_NTVDM_STARTUP_CONFIGURATION_POLICY_V1_MAGIC;
    policy->abi_version = BX_NTVDM_STARTUP_CONFIGURATION_POLICY_V1_VERSION;
    policy->struct_bytes = (uint32_t)sizeof(*policy);
    policy->source = source;
    policy->mutation_mode = mode;
    policy->permitted_mode_mask = permitted_mask;
    return bx_ntvdm_startup_configuration_policy_v1_valid(policy);
}
