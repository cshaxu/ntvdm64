#include "bx_ntvdm_dem_profile_consumer_v1.h"

static int resolves(uint32_t mode, uint32_t expected)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_dem_profile_consumer_v1 consumer;
    uint32_t result = 0u;
    bx_ntvdm_mutation_profile_v1_initialize(&profile, mode);
    if (!bx_ntvdm_dem_profile_consumer_v1_register_class(&profile,
            BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu) ||
        !bx_ntvdm_dem_profile_consumer_v1_initialize(&consumer, &profile) ||
        !bx_ntvdm_dem_profile_consumer_v1_resolve(&consumer,
            BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, &result) ||
        result != expected) return 0;
    return !bx_ntvdm_dem_profile_consumer_v1_resolve(&consumer,
        BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, &result);
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_dem_profile_consumer_v1 consumer;
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
    if (!bx_ntvdm_dem_profile_consumer_v1_initialize(&consumer, &profile) ||
        bx_ntvdm_dem_profile_consumer_v1_resolve(&consumer,
            BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0)) return 2;
    return 0;
}
