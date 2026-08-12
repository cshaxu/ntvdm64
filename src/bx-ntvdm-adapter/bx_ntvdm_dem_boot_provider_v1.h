#ifndef BX_NTVDM_DEM_BOOT_PROVIDER_V1_H
#define BX_NTVDM_DEM_BOOT_PROVIDER_V1_H

#include "bx_ntvdm_bop_provider_registry_v1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The caller supplies the already profile-selected DOS drive index, never a
 * host path or host drive. The result is the OpenNT one-based AL value. */
int bx_ntvdm_dem_boot_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    uint32_t profile_drive_index,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif
