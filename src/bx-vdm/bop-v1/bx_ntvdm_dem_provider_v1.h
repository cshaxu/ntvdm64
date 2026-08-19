#ifndef BX_NTVDM_DEM_PROVIDER_V1_H
#define BX_NTVDM_DEM_PROVIDER_V1_H

#include "bx_ntvdm_dem_plane_v1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* First DEM provider slice. It restores only the exact original
 * demNotYetImplemented guest contract; it is not a file, fast-read, or DOS
 * provider. Returns one only when it owned the supplied service. */
int bx_ntvdm_dem_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif
