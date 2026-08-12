#ifndef BX_NTVDM_DEM_DRIVE_PROVIDER_V1_H
#define BX_NTVDM_DEM_DRIVE_PROVIDER_V1_H

#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_host_drive_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DEM drive-projection provider. It accepts only an immutable session
 * snapshot supplied by the CLI capability layer; it never scans host drives
 * during a guest BOP. */
int bx_ntvdm_dem_drive_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif
