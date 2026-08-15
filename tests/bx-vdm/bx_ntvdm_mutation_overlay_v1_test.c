#include "bx_ntvdm_mutation_overlay_v1.h"

#include <string.h>

static int overlay_profile(bx_ntvdm_mutation_profile_v1 *profile, uint32_t mode)
{
    bx_ntvdm_mutation_owner_registration_v1 owner;
    bx_ntvdm_mutation_profile_v1_initialize(profile, mode);
    owner.owner_id = BX_NTVDM_MUTATION_OWNER_V1_DEM;
    owner.mutation_class = BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT;
    owner.permitted_mode_mask = 0x0fu;
    owner.reserved0 = 0u;
    return bx_ntvdm_mutation_profile_v1_register_owner(profile, &owner);
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_mutation_overlay_v1 overlay;
    uint8_t value[] = { 1u, 2u, 3u };
    uint8_t output[3] = { 0u, 0u, 0u };
    uint32_t output_bytes = 0u;
    if (!overlay_profile(&profile, BX_NTVDM_MUTATION_MODE_V1_OVERLAY) ||
        !bx_ntvdm_mutation_overlay_v1_initialize(&overlay, &profile) ||
        !bx_ntvdm_mutation_overlay_v1_record(&overlay,
            BX_NTVDM_MUTATION_OWNER_V1_DEM,
            BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 7u, value,
            sizeof(value)) ||
        bx_ntvdm_mutation_overlay_v1_record(&overlay,
            BX_NTVDM_MUTATION_OWNER_V1_DEM,
            BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 7u, value,
            sizeof(value)) ||
        !bx_ntvdm_mutation_overlay_v1_lookup(&overlay,
            BX_NTVDM_MUTATION_OWNER_V1_DEM,
            BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 7u, output,
            sizeof(output), &output_bytes) || output_bytes != sizeof(value) ||
        memcmp(output, value, sizeof(value)) != 0) return 1;
    if (bx_ntvdm_mutation_overlay_v1_record(&overlay,
            BX_NTVDM_MUTATION_OWNER_V1_COMMAND,
            BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 8u, value,
            sizeof(value))) return 2;
    bx_ntvdm_mutation_overlay_v1_teardown(&overlay);
    if (overlay.magic != 0u || overlay.record_count != 0u ||
        bx_ntvdm_mutation_overlay_v1_valid(&overlay)) return 3;
    if (!overlay_profile(&profile, BX_NTVDM_MUTATION_MODE_V1_DIRECT) ||
        bx_ntvdm_mutation_overlay_v1_initialize(&overlay, &profile)) return 4;
    if (!overlay_profile(&profile, BX_NTVDM_MUTATION_MODE_V1_READONLY) ||
        bx_ntvdm_mutation_overlay_v1_initialize(&overlay, &profile)) return 5;
    if (!overlay_profile(&profile, BX_NTVDM_MUTATION_MODE_V1_VIRTUAL) ||
        bx_ntvdm_mutation_overlay_v1_initialize(&overlay, &profile)) return 6;
    return 0;
}
