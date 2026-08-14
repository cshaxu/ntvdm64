#ifndef BX_NTVDM_DEM_FASTIO_NOOP_SERVICE_H
#define BX_NTVDM_DEM_FASTIO_NOOP_SERVICE_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_instruction_window_abi.h"

int bx_ntvdm_dem_fastio_noop_service_v1_dispatch(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
