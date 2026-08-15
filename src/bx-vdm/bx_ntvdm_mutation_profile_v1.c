#include "bx_ntvdm_mutation_profile_v1.h"

#include <string.h>

static int valid_mode(uint32_t mode)
{
    return mode >= BX_NTVDM_MUTATION_MODE_V1_DIRECT &&
        mode <= BX_NTVDM_MUTATION_MODE_V1_VIRTUAL;
}

static int valid_class(uint32_t mutation_class)
{
    return mutation_class <= BX_NTVDM_MUTATION_CLASS_V1_IPC_NETWORK;
}

static uint32_t mode_bit(uint32_t mode)
{
    return valid_mode(mode) ? (1u << (mode - 1u)) : 0u;
}

void bx_ntvdm_mutation_profile_v1_initialize(
    bx_ntvdm_mutation_profile_v1 *profile, uint32_t mode)
{
    if (profile == 0) return;
    memset(profile, 0, sizeof(*profile));
    profile->magic = BX_NTVDM_MUTATION_PROFILE_V1_MAGIC;
    profile->abi_version = BX_NTVDM_MUTATION_PROFILE_V1_VERSION;
    profile->struct_bytes = sizeof(*profile);
    profile->mode = mode;
}

int bx_ntvdm_mutation_profile_v1_valid(
    const bx_ntvdm_mutation_profile_v1 *profile)
{
    uint32_t index;
    if (profile == 0 || profile->magic != BX_NTVDM_MUTATION_PROFILE_V1_MAGIC ||
        profile->abi_version != BX_NTVDM_MUTATION_PROFILE_V1_VERSION ||
        profile->struct_bytes != sizeof(*profile) || !valid_mode(profile->mode) ||
        profile->owner_count > BX_NTVDM_MUTATION_PROFILE_V1_MAX_OWNERS ||
        profile->reserved0 != 0u) return 0;
    for (index = 0u; index < profile->owner_count; ++index) {
        const bx_ntvdm_mutation_owner_registration_v1 *entry =
            &profile->owners[index];
        uint32_t prior;
        if (entry->owner_id == 0u || !valid_class(entry->mutation_class) ||
            entry->permitted_mode_mask == 0u ||
            (entry->permitted_mode_mask & ~0x0fu) != 0u ||
            entry->reserved0 != 0u) return 0;
        for (prior = 0u; prior < index; ++prior) {
            if (profile->owners[prior].owner_id == entry->owner_id &&
                profile->owners[prior].mutation_class == entry->mutation_class)
                return 0;
        }
    }
    for (; index < BX_NTVDM_MUTATION_PROFILE_V1_MAX_OWNERS; ++index) {
        const bx_ntvdm_mutation_owner_registration_v1 *entry =
            &profile->owners[index];
        if (entry->owner_id != 0u || entry->mutation_class != 0u ||
            entry->permitted_mode_mask != 0u || entry->reserved0 != 0u) return 0;
    }
    return 1;
}

int bx_ntvdm_mutation_profile_v1_register_owner(
    bx_ntvdm_mutation_profile_v1 *profile,
    const bx_ntvdm_mutation_owner_registration_v1 *registration)
{
    uint32_t index;
    if (profile == 0 || registration == 0 ||
        !bx_ntvdm_mutation_profile_v1_valid(profile) ||
        registration->owner_id == 0u || !valid_class(registration->mutation_class) ||
        registration->permitted_mode_mask == 0u ||
        (registration->permitted_mode_mask & ~0x0fu) != 0u ||
        registration->reserved0 != 0u ||
        profile->owner_count >= BX_NTVDM_MUTATION_PROFILE_V1_MAX_OWNERS) return 0;
    for (index = 0u; index < profile->owner_count; ++index) {
        if (profile->owners[index].owner_id == registration->owner_id &&
            profile->owners[index].mutation_class == registration->mutation_class)
            return 0;
    }
    profile->owners[profile->owner_count++] = *registration;
    return bx_ntvdm_mutation_profile_v1_valid(profile);
}

int bx_ntvdm_mutation_profile_v1_authorize(
    const bx_ntvdm_mutation_profile_v1 *profile, uint32_t owner_id,
    uint32_t mutation_class, uint32_t *mode_out)
{
    uint32_t index;
    if (mode_out == 0 || !bx_ntvdm_mutation_profile_v1_valid(profile) ||
        owner_id == 0u || !valid_class(mutation_class)) return 0;
    for (index = 0u; index < profile->owner_count; ++index) {
        const bx_ntvdm_mutation_owner_registration_v1 *entry =
            &profile->owners[index];
        if (entry->owner_id == owner_id &&
            entry->mutation_class == mutation_class) {
            if ((entry->permitted_mode_mask & mode_bit(profile->mode)) == 0u)
                return 0;
            *mode_out = profile->mode;
            return 1;
        }
    }
    return 0;
}

int bx_ntvdm_mutation_profile_v1_resolve(
    const bx_ntvdm_mutation_profile_v1 *profile, uint32_t owner_id,
    uint32_t mutation_class, uint32_t *policy_result_out)
{
    uint32_t mode;
    if (policy_result_out == 0 || !bx_ntvdm_mutation_profile_v1_authorize(
            profile, owner_id, mutation_class, &mode)) return 0;
    switch (mode) {
    case BX_NTVDM_MUTATION_MODE_V1_DIRECT:
        *policy_result_out = BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST;
        return 1;
    case BX_NTVDM_MUTATION_MODE_V1_READONLY:
        *policy_result_out = BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY;
        return 1;
    case BX_NTVDM_MUTATION_MODE_V1_OVERLAY:
        *policy_result_out = BX_NTVDM_MUTATION_POLICY_V1_USE_OVERLAY;
        return 1;
    case BX_NTVDM_MUTATION_MODE_V1_VIRTUAL:
        *policy_result_out = BX_NTVDM_MUTATION_POLICY_V1_USE_VIRTUAL;
        return 1;
    default:
        return 0;
    }
}
