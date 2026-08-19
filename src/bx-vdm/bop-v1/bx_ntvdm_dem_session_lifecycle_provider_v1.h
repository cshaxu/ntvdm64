#ifndef BX_NTVDM_DEM_SESSION_LIFECYCLE_PROVIDER_V1_H
#define BX_NTVDM_DEM_SESSION_LIFECYCLE_PROVIDER_V1_H

#include "bx_ntvdm_boot_namespace_provider_v1.h"
#include "bx_ntvdm_dem_plane_v1.h"
#include "bx_ntvdm_dem_whole_provider_v1.h"

/* Source-derived no-VDD equivalent of demEntryDosApp and finite-profile
 * demTerminatePDB cleanup. It intentionally has no VDD, device, DOS or
 * guest-memory ABI. `whole_provider` is optional because the original void
 * notification also occurs before a local-file provider is admitted. */
int bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch(
    bx_ntvdm_boot_namespace_provider_v1 *provider,
    bx_ntvdm_dem_whole_provider_v1 *whole_provider,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result);

#endif
