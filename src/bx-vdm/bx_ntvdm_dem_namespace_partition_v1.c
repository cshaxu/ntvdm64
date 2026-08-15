#include "bx_ntvdm_dem_namespace_partition_v1.h"

int bx_ntvdm_dem_namespace_partition_v1_owns_service(uint8_t service)
{
    return service == 0x01u || service == 0x03u || service == 0x04u ||
        service == 0x05u || service == 0x06u || service == 0x12u ||
        service == 0x17u || service == 0x22u || service == 0x44u;
}

int bx_ntvdm_dem_namespace_partition_v1_resolve(
    const bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const char *oem_path, uint8_t *drive_out,
    wchar_t relative_out[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE],
    uint32_t mutation_class, uint32_t *policy_out)
{
    if (drive_out != 0) *drive_out = 0u;
    if (policy_out != 0) *policy_out = 0u;
    if (!provider || !oem_path || !drive_out || !relative_out || !policy_out ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider) ||
        !bx_ntvdm_dem_namespace_partition_v1_owns_service(service)) return 0;
    if (!bx_ntvdm_dem_profile_consumer_v1_resolve(&provider->files.profile,
            mutation_class, policy_out)) return 0;
    return bx_ntvdm_dem_path_v1_resolve(oem_path, provider->cwd, drive_out,
        relative_out) == BX_NTVDM_DEM_PATH_V1_OK;
}
