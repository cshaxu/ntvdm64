#include "bx_ntvdm_dem_whole_provider_v1.h"

#include <wctype.h>

static int profile_for(bx_ntvdm_mutation_profile_v1 *profile)
{
    bx_ntvdm_mutation_profile_v1_initialize(profile,
        BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu);
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_host_drive_snapshot_v1 snapshot = {0};
    bx_ntvdm_host_namespace_v1 space;
    bx_ntvdm_dem_cwd_context_v1 cwd;
    bx_ntvdm_dem_whole_provider_v1 provider;
    wchar_t temporary[MAX_PATH], short_name[MAX_PATH];
    uint8_t drive;
    uint32_t service;
    int failed = 0;

    if (GetTempPathW(MAX_PATH, temporary) == 0u ||
        GetTempFileNameW(temporary, L"n64", 0u, temporary) == 0u ||
        GetShortPathNameW(temporary, short_name, MAX_PATH) == 0u ||
        short_name[1] != L':') return 1;
    drive = (uint8_t)(towupper((wint_t)short_name[0]) - L'A');
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = UINT32_C(1) << drive;
    snapshot.admitted_mask = snapshot.present_mask;
    snapshot.types[drive] = DRIVE_FIXED;
    if (!profile_for(&profile) || !bx_ntvdm_host_namespace_v1_initialize(&space,
            &snapshot) || !bx_ntvdm_dem_cwd_context_v1_initialize(&cwd,
            &profile) || !bx_ntvdm_dem_whole_provider_v1_initialize(&provider,
            &profile, &space, &cwd)) failed = 1;
    for (service = 0u; !failed && service < 0x49u; ++service) {
        const int expected = service == 0x00u || service == 0x01u ||
            service == 0x02u || service == 0x03u || service == 0x04u ||
            service == 0x05u || service == 0x06u || service == 0x07u ||
            service == 0x08u || service == 0x09u || service == 0x0au ||
            service == 0x0bu || service == 0x0cu || service == 0x12u ||
            service == 0x16u || service == 0x17u || service == 0x1eu ||
            service == 0x20u || service == 0x22u || service == 0x27u ||
            service == 0x2cu || service == 0x2du || service == 0x2eu ||
            service == 0x2fu || service == 0x30u || service == 0x31u ||
            service == 0x44u || service == 0x47u || service == 0x48u;
        if (bx_ntvdm_dem_whole_provider_v1_owns_service((uint8_t)service) != expected)
            failed = 1;
    }
    bx_ntvdm_dem_whole_provider_v1_teardown(&provider);
    failed |= bx_ntvdm_dem_whole_provider_v1_valid(&provider) != 0;
    bx_ntvdm_host_namespace_v1_release(&space);
    DeleteFileW(temporary);
    return failed ? 2 : 0;
}
