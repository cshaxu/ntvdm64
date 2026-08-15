#include "bx_ntvdm_mutation_profile_v1.h"

static int profile_case(uint32_t mode)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_mutation_owner_registration_v1 owner;
    uint32_t selected = 0u;
    owner.owner_id = BX_NTVDM_MUTATION_OWNER_V1_DEM;
    owner.mutation_class = BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT;
    owner.permitted_mode_mask = 0x0fu;
    owner.reserved0 = 0u;
    bx_ntvdm_mutation_profile_v1_initialize(&profile, mode);
    if (!bx_ntvdm_mutation_profile_v1_valid(&profile) ||
        !bx_ntvdm_mutation_profile_v1_register_owner(&profile, &owner) ||
        !bx_ntvdm_mutation_profile_v1_authorize(&profile, owner.owner_id,
            owner.mutation_class, &selected) || selected != mode) return 0;
    return !bx_ntvdm_mutation_profile_v1_authorize(&profile,
        BX_NTVDM_MUTATION_OWNER_V1_COMMAND, owner.mutation_class, &selected);
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_mutation_owner_registration_v1 owner;
    uint32_t selected;
    if (!profile_case(BX_NTVDM_MUTATION_MODE_V1_DIRECT) ||
        !profile_case(BX_NTVDM_MUTATION_MODE_V1_READONLY) ||
        !profile_case(BX_NTVDM_MUTATION_MODE_V1_OVERLAY) ||
        !profile_case(BX_NTVDM_MUTATION_MODE_V1_VIRTUAL)) return 1;
    bx_ntvdm_mutation_profile_v1_initialize(&profile, 0u);
    if (bx_ntvdm_mutation_profile_v1_valid(&profile)) return 2;
    bx_ntvdm_mutation_profile_v1_initialize(&profile,
        BX_NTVDM_MUTATION_MODE_V1_READONLY);
    owner.owner_id = BX_NTVDM_MUTATION_OWNER_V1_COMMAND;
    owner.mutation_class = BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT;
    owner.permitted_mode_mask = 1u <<
        (BX_NTVDM_MUTATION_MODE_V1_READONLY - 1u);
    owner.reserved0 = 0u;
    if (!bx_ntvdm_mutation_profile_v1_register_owner(&profile, &owner) ||
        bx_ntvdm_mutation_profile_v1_register_owner(&profile, &owner) ||
        bx_ntvdm_mutation_profile_v1_authorize(&profile, owner.owner_id,
            BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, &selected) ||
        !bx_ntvdm_mutation_profile_v1_authorize(&profile, owner.owner_id,
            owner.mutation_class, &selected) ||
        selected != BX_NTVDM_MUTATION_MODE_V1_READONLY) return 3;
    owner.owner_id = BX_NTVDM_MUTATION_OWNER_V1_REDIR;
    owner.permitted_mode_mask = 1u <<
        (BX_NTVDM_MUTATION_MODE_V1_DIRECT - 1u);
    if (!bx_ntvdm_mutation_profile_v1_register_owner(&profile, &owner) ||
        bx_ntvdm_mutation_profile_v1_authorize(&profile, owner.owner_id,
            owner.mutation_class, &selected)) return 4;
    return 0;
}
