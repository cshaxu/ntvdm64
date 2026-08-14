#ifndef BX_NTVDM_DEBUGGER_PACKAGE_FACADE_V1_H
#define BX_NTVDM_DEBUGGER_PACKAGE_FACADE_V1_H

#include "bx_ntvdm_system_plane_v1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The CLI deliberately supplies no debugger event plane.  This facade owns
 * only the package-wide deferred stop; it never reads the Debugger stack ABI. */
int bx_ntvdm_debugger_package_facade_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif
