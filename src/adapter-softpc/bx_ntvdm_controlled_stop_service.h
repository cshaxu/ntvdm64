#ifndef BX_NTVDM_CONTROLLED_STOP_SERVICE_H
#define BX_NTVDM_CONTROLLED_STOP_SERVICE_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

int bx_ntvdm_controlled_stop_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
