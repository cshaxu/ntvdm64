#include "bx_ntvdm_dem_drive_view_provider_v1.h"

#include <string.h>

void bx_ntvdm_dem_drive_view_provider_v1_clear(
    bx_ntvdm_dem_drive_view_provider_v1 *provider)
{
    if (provider == 0) return;
    memset(provider, 0, sizeof(*provider));
}

int bx_ntvdm_dem_drive_view_provider_v1_valid(
    const bx_ntvdm_dem_drive_view_provider_v1 *provider)
{
    return provider != 0 &&
        provider->magic == BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_MAGIC &&
        provider->abi_version == BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_VERSION &&
        provider->struct_bytes == sizeof(*provider) && provider->initialized == 1u &&
        provider->has_mutation_profile <= 1u && provider->has_boot_drive <= 1u &&
        (!provider->gset.has_drive_snapshot ||
            bx_ntvdm_host_drive_snapshot_v1_valid(&provider->gset.drive_snapshot)) &&
        (!provider->gset.has_volume_snapshot ||
            bx_ntvdm_host_volume_snapshot_v1_valid(&provider->gset.volume_snapshot)) &&
        (!provider->has_boot_drive || (provider->boot_drive_index < 26u &&
            provider->gset.has_drive_snapshot &&
            (provider->gset.drive_snapshot.admitted_mask &
                (UINT32_C(1) << provider->boot_drive_index)) != 0u)) &&
        (!provider->has_mutation_profile ||
            (bx_ntvdm_dem_profile_consumer_v1_valid(&provider->mutation_profile) &&
             bx_ntvdm_dem_cwd_context_v1_valid(&provider->cwd)));
}

int bx_ntvdm_dem_drive_view_provider_v1_initialize(
    bx_ntvdm_dem_drive_view_provider_v1 *provider)
{
    if (provider == 0) return 0;
    bx_ntvdm_dem_drive_view_provider_v1_clear(provider);
    provider->magic = BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_MAGIC;
    provider->abi_version = BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_VERSION;
    provider->struct_bytes = (uint32_t)sizeof(*provider);
    bx_ntvdm_dem_gset_plane_v1_clear(&provider->gset);
    provider->initialized = 1u;
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider);
}

int bx_ntvdm_dem_drive_view_provider_v1_set_drive_snapshot(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot)
{
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider) &&
        bx_ntvdm_dem_gset_plane_v1_set_drive_snapshot(&provider->gset, snapshot) &&
        bx_ntvdm_dem_drive_view_provider_v1_valid(provider);
}

int bx_ntvdm_dem_drive_view_provider_v1_set_volume_snapshot(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_host_volume_snapshot_v1 *snapshot)
{
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider) &&
        bx_ntvdm_dem_gset_plane_v1_set_volume_snapshot(&provider->gset, snapshot) &&
        bx_ntvdm_dem_drive_view_provider_v1_valid(provider);
}

int bx_ntvdm_dem_drive_view_provider_v1_set_mutation_profile(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_mutation_profile_v1 *profile)
{
    if (!bx_ntvdm_dem_drive_view_provider_v1_valid(provider) || profile == 0 ||
        provider->has_mutation_profile ||
        !bx_ntvdm_dem_profile_consumer_v1_initialize(&provider->mutation_profile, profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&provider->cwd, profile)) return 0;
    provider->has_mutation_profile = 1u;
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider);
}

int bx_ntvdm_dem_drive_view_provider_v1_set_boot_drive(
    bx_ntvdm_dem_drive_view_provider_v1 *provider, uint32_t drive_index)
{
    if (!bx_ntvdm_dem_drive_view_provider_v1_valid(provider) ||
        provider->has_boot_drive || drive_index >= 26u ||
        !provider->gset.has_drive_snapshot ||
        (provider->gset.drive_snapshot.admitted_mask & (UINT32_C(1) << drive_index)) == 0u)
        return 0;
    provider->boot_drive_index = drive_index;
    provider->has_boot_drive = 1u;
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider);
}
