#ifndef BX_NTVDM_DEM_READONLY_NAMESPACE_FAILURE_PROVIDER_V1_H
#define BX_NTVDM_DEM_READONLY_NAMESPACE_FAILURE_PROVIDER_V1_H

#include "bx_ntvdm_dem_package_facade_v1.h"

/* Source-derived contained-filesystem refusal.  This is deliberately limited
 * to path/namespace mutations; it is not the general DEM fallback and it
 * does not emulate a DOS filesystem. */
int bx_ntvdm_dem_readonly_namespace_failure_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_package_route_v1 *route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
