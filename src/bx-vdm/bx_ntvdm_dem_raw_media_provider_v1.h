#ifndef BX_NTVDM_DEM_RAW_MEDIA_PROVIDER_V1_H
#define BX_NTVDM_DEM_RAW_MEDIA_PROVIDER_V1_H

#include "bx_ntvdm_dem_package_facade_v1.h"

/* Source-derived terminal half of demioctl.c/demdasd.c for the no-device
 * CLI profile.  Metadata-only IOCTL 08/09 stays in its existing provider. */
int bx_ntvdm_dem_raw_media_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_package_route_v1 *route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
