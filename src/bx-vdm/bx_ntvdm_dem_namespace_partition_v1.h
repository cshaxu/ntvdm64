#ifndef BX_NTVDM_DEM_NAMESPACE_PARTITION_V1_H
#define BX_NTVDM_DEM_NAMESPACE_PARTITION_V1_H

#include "bx_ntvdm_dem_whole_provider_v1.h"

/* Whole source partition for demfile.c/demdir.c pathname operations. The
 * caller supplies only already-gathered OEM strings; this layer has no BOP
 * window, guest pointer or Bochs dependency. */
int bx_ntvdm_dem_namespace_partition_v1_owns_service(uint8_t service);
int bx_ntvdm_dem_namespace_partition_v1_resolve(
    const bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const char *oem_path, uint8_t *drive_out,
    wchar_t relative_out[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE],
    uint32_t mutation_class, uint32_t *policy_out);

#endif
