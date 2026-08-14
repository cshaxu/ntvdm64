#ifndef BX_NTVDM_DEM_CLI_UNAVAILABLE_PROVIDER_V1_H
#define BX_NTVDM_DEM_CLI_UNAVAILABLE_PROVIDER_V1_H

#include "bx_ntvdm_dem_package_facade_v1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Final DEM package boundary for a capability that the non-invasive CLI
 * profile deliberately does not supply.  It is source-derived from
 * demClientError(): a missing host error becomes ERROR_ACCESS_DENIED (5),
 * CF is set, and AX carries the DOS-visible failure. */
int bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_package_route_v1 *route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif
