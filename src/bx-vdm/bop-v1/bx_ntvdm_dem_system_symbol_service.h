#ifndef BX_NTVDM_DEM_SYSTEM_SYMBOL_SERVICE_H
#define BX_NTVDM_DEM_SYSTEM_SYMBOL_SERVICE_H

#include "bx_ntvdm_exception_abi.h"

/* Source-derived non-debug disposition of demSystemSymbolOp.  It deliberately
 * has no symbol store, debugger callback, path, or host-resource surface. */
int bx_ntvdm_dem_system_symbol_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result);

#endif
