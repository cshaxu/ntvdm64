#include "bop-v1/bx_ntvdm_mutation_profile_v1.h"

static int resolves(uint32_t mode, uint32_t expected)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_mutation_owner_registration_v1 owner;
    uint32_t result = 0u;
    bx_ntvdm_mutation_profile_v1_initialize(&profile, mode);
    owner.owner_id = BX_NTVDM_MUTATION_OWNER_V1_DEM;
    owner.mutation_class = BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA;
    owner.permitted_mode_mask = 0x0fu;
    owner.reserved0 = 0u;
    return bx_ntvdm_mutation_profile_v1_register_owner(&profile, &owner) &&
        bx_ntvdm_mutation_profile_v1_resolve(&profile, owner.owner_id,
            owner.mutation_class, &result) && result == expected &&
        !bx_ntvdm_mutation_profile_v1_resolve(&profile,
            BX_NTVDM_MUTATION_OWNER_V1_COMMAND, owner.mutation_class, &result);
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_mutation_owner_registration_v1 owner;
    uint32_t result = 0u;
    if (!resolves(BX_NTVDM_MUTATION_MODE_V1_DIRECT,
            BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST) ||
        !resolves(BX_NTVDM_MUTATION_MODE_V1_READONLY,
            BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY) ||
        !resolves(BX_NTVDM_MUTATION_MODE_V1_OVERLAY,
            BX_NTVDM_MUTATION_POLICY_V1_USE_OVERLAY) ||
        !resolves(BX_NTVDM_MUTATION_MODE_V1_VIRTUAL,
            BX_NTVDM_MUTATION_POLICY_V1_USE_VIRTUAL)) return 1;
    bx_ntvdm_mutation_profile_v1_initialize(&profile,
        BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    owner.owner_id = BX_NTVDM_MUTATION_OWNER_V1_DEM;
    owner.mutation_class = BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA;
    owner.permitted_mode_mask = 1u <<
        (BX_NTVDM_MUTATION_MODE_V1_READONLY - 1u);
    owner.reserved0 = 0u;
    if (!bx_ntvdm_mutation_profile_v1_register_owner(&profile, &owner) ||
        bx_ntvdm_mutation_profile_v1_resolve(&profile, owner.owner_id,
            owner.mutation_class, &result)) return 2;
    return 0;
}
