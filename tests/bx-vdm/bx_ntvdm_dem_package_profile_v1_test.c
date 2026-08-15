#include "bx_ntvdm_dem_package_session_v1.h"

#include <string.h>

static int resolves(uint32_t mode, uint32_t expected)
{
    bx_ntvdm_boot_namespace_plane_v1 plane;
    bx_ntvdm_dem_package_session_v1 session;
    bx_ntvdm_mutation_profile_v1 profile;
    uint32_t result = 0u;
    memset(&plane, 0, sizeof(plane));
    bx_ntvdm_mutation_profile_v1_initialize(&profile, mode);
    if (!bx_ntvdm_dem_profile_consumer_v1_register_class(&profile,
            BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu) ||
        !bx_ntvdm_dem_package_session_v1_initialize(&session, &plane) ||
        !bx_ntvdm_dem_package_session_v1_set_mutation_profile(&session,
            &profile) ||
        bx_ntvdm_dem_package_session_v1_set_mutation_profile(&session,
            &profile) ||
        !bx_ntvdm_dem_package_session_v1_resolve_mutation_class(&session,
            BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, &result) ||
        result != expected) return 0;
    return !bx_ntvdm_dem_package_session_v1_resolve_mutation_class(&session,
        BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, &result);
}

int main(void)
{
    return resolves(BX_NTVDM_MUTATION_MODE_V1_DIRECT,
            BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST) &&
        resolves(BX_NTVDM_MUTATION_MODE_V1_READONLY,
            BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY) &&
        resolves(BX_NTVDM_MUTATION_MODE_V1_OVERLAY,
            BX_NTVDM_MUTATION_POLICY_V1_USE_OVERLAY) &&
        resolves(BX_NTVDM_MUTATION_MODE_V1_VIRTUAL,
            BX_NTVDM_MUTATION_POLICY_V1_USE_VIRTUAL) ? 0 : 1;
}
