#ifndef BX_NTVDM_DEM_DEBUG_SERVICE_H
#define BX_NTVDM_DEM_DEBUG_SERVICE_H

#include "bx_ntvdm_exception_abi.h"

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_dem_debug_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result);
#ifdef __cplusplus
}
#endif
#endif
