#ifndef BX_NTVDM_EMM_UNAVAILABLE_SERVICE_H
#define BX_NTVDM_EMM_UNAVAILABLE_SERVICE_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Guest-visible unavailable completion for InitSpcEmm's BOP 66 probe.
 * This is not an EMS implementation: BX=0 makes the original guest retain
 * its NoEmmServices branch. */
int bx_ntvdm_emm_unavailable_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif
