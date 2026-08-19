#include "bop-v1/bx_ntvdm_startup_configuration_policy_v1.h"

static int initialize_profile(bx_ntvdm_mutation_profile_v1 *profile,
    uint32_t mode, uint32_t permitted_mode_mask)
{
    bx_ntvdm_mutation_owner_registration_v1 registration;
    bx_ntvdm_mutation_profile_v1_initialize(profile, mode);
    registration.owner_id = BX_NTVDM_MUTATION_OWNER_V1_COMMAND;
    registration.mutation_class = BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT;
    registration.permitted_mode_mask = permitted_mode_mask;
    registration.reserved0 = 0u;
    return bx_ntvdm_mutation_profile_v1_register_owner(profile, &registration);
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_startup_configuration_policy_v1 policy;

    if (!initialize_profile(&profile, BX_NTVDM_MUTATION_MODE_V1_DIRECT, 0x0fu) ||
        !bx_ntvdm_startup_configuration_policy_v1_initialize(&policy, &profile,
            BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_CONTAINED_FIXTURE) ||
        !bx_ntvdm_startup_configuration_policy_v1_valid(&policy) ||
        policy.mutation_mode != BX_NTVDM_MUTATION_MODE_V1_DIRECT ||
        policy.permitted_mode_mask != 0x0fu) return 1;

    if (!bx_ntvdm_startup_configuration_policy_v1_initialize(&policy, &profile,
            BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_EXPLICIT_HOST) ||
        !bx_ntvdm_startup_configuration_policy_v1_valid(&policy)) return 2;

    if (bx_ntvdm_startup_configuration_policy_v1_initialize(&policy, &profile,
            BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_VIRTUAL_VOLUME) ||
        bx_ntvdm_startup_configuration_policy_v1_valid(&policy)) return 3;

    if (!initialize_profile(&profile, BX_NTVDM_MUTATION_MODE_V1_VIRTUAL, 0x0fu) ||
        !bx_ntvdm_startup_configuration_policy_v1_initialize(&policy, &profile,
            BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_VIRTUAL_VOLUME) ||
        !bx_ntvdm_startup_configuration_policy_v1_valid(&policy) ||
        policy.permitted_mode_mask != 8u) return 4;

    if (!initialize_profile(&profile, BX_NTVDM_MUTATION_MODE_V1_READONLY, 0x0fu) ||
        !bx_ntvdm_startup_configuration_policy_v1_initialize(&policy, &profile,
            BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_EXPLICIT_HOST) ||
        policy.mutation_mode != BX_NTVDM_MUTATION_MODE_V1_READONLY) return 5;
    if (!initialize_profile(&profile, BX_NTVDM_MUTATION_MODE_V1_OVERLAY, 0x0fu) ||
        !bx_ntvdm_startup_configuration_policy_v1_initialize(&policy, &profile,
            BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_CONTAINED_FIXTURE) ||
        policy.mutation_mode != BX_NTVDM_MUTATION_MODE_V1_OVERLAY) return 6;

    policy.reserved0 = 1u;
    if (bx_ntvdm_startup_configuration_policy_v1_valid(&policy)) return 7;
    bx_ntvdm_startup_configuration_policy_v1_clear(&policy);
    if (bx_ntvdm_startup_configuration_policy_v1_valid(&policy)) return 8;

    bx_ntvdm_mutation_profile_v1_initialize(&profile,
        BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    if (bx_ntvdm_startup_configuration_policy_v1_initialize(&policy, &profile,
            BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_CONTAINED_FIXTURE) ||
        bx_ntvdm_startup_configuration_policy_v1_valid(&policy)) return 9;
    return 0;
}
