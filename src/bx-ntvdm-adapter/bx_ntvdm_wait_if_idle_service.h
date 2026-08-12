#ifndef BX_NTVDM_WAIT_IF_IDLE_SERVICE_H
#define BX_NTVDM_WAIT_IF_IDLE_SERVICE_H

#include "bx_ntvdm_exception_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Source-shaped BOP 5A immediate-return branch.  This service owns neither
 * an idle scheduler nor VDD state; it only expresses the guest-visible
 * continuation when the historical idle prerequisites are absent. */
int bx_ntvdm_wait_if_idle_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result);

#ifdef __cplusplus
}
#endif

#endif
