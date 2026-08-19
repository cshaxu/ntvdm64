#ifndef BX_NTVDM_DEM_IOCTL_METADATA_PROVIDER_V1_H
#define BX_NTVDM_DEM_IOCTL_METADATA_PROVIDER_V1_H

#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_dem_plane_v1.h"
#include "bx_ntvdm_host_drive_policy.h"
#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

/* Source-derived demIoctlChangeable replacement. It consumes only the
 * immutable CLI snapshot and preserves the AL=08/09 register contracts. */
int bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result);

#endif
