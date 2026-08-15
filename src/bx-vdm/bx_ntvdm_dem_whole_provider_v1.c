#include "bx_ntvdm_dem_whole_provider_v1.h"

#include <string.h>

int bx_ntvdm_dem_whole_provider_v1_owns_service(uint8_t service)
{
    switch (service) {
    case 0x00u: case 0x01u: case 0x02u: case 0x03u: case 0x04u:
    case 0x05u: case 0x06u: case 0x07u: case 0x08u: case 0x09u:
    case 0x0au: case 0x0bu: case 0x0cu: case 0x12u: case 0x16u:
    case 0x17u: case 0x1eu: case 0x20u: case 0x22u: case 0x27u:
    case 0x2cu: case 0x2du: case 0x2eu: case 0x2fu: case 0x30u:
    case 0x31u: case 0x44u: case 0x47u: case 0x48u:
        return 1;
    default:
        return 0;
    }
}

int bx_ntvdm_dem_whole_provider_v1_valid(
    const bx_ntvdm_dem_whole_provider_v1 *provider)
{
    return provider != 0 &&
        provider->magic == BX_NTVDM_DEM_WHOLE_PROVIDER_V1_MAGIC &&
        provider->abi_version == BX_NTVDM_DEM_WHOLE_PROVIDER_V1_VERSION &&
        provider->struct_bytes == sizeof(*provider) && provider->reserved0 == 0u &&
        bx_ntvdm_host_namespace_v1_valid(provider->host_namespace) &&
        bx_ntvdm_dem_cwd_context_v1_valid(provider->cwd) &&
        bx_ntvdm_dem_file_session_v1_valid(&provider->files) &&
        bx_ntvdm_dem_local_file_backend_v1_valid(&provider->local_files);
}

int bx_ntvdm_dem_whole_provider_v1_initialize(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_mutation_profile_v1 *profile,
    const bx_ntvdm_host_namespace_v1 *host_namespace,
    const bx_ntvdm_dem_cwd_context_v1 *cwd)
{
    if (provider == 0 || profile == 0 ||
        !bx_ntvdm_mutation_profile_v1_valid(profile) ||
        !bx_ntvdm_host_namespace_v1_valid(host_namespace) ||
        !bx_ntvdm_dem_cwd_context_v1_valid(cwd)) return 0;
    memset(provider, 0, sizeof(*provider));
    provider->magic = BX_NTVDM_DEM_WHOLE_PROVIDER_V1_MAGIC;
    provider->abi_version = BX_NTVDM_DEM_WHOLE_PROVIDER_V1_VERSION;
    provider->struct_bytes = sizeof(*provider);
    provider->host_namespace = host_namespace;
    provider->cwd = cwd;
    if (!bx_ntvdm_dem_file_session_v1_initialize(&provider->files, profile) ||
        !bx_ntvdm_dem_local_file_backend_v1_initialize(&provider->local_files,
            &provider->files, host_namespace, cwd)) {
        bx_ntvdm_dem_whole_provider_v1_teardown(provider);
        return 0;
    }
    return bx_ntvdm_dem_whole_provider_v1_valid(provider);
}

void bx_ntvdm_dem_whole_provider_v1_teardown(
    bx_ntvdm_dem_whole_provider_v1 *provider)
{
    if (provider == 0) return;
    bx_ntvdm_dem_file_session_v1_teardown(&provider->files);
    memset(provider, 0, sizeof(*provider));
}
