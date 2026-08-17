#ifndef BX_NTVDM_STARTUP_MACHINE_INTERRUPT_V1_H
#define BX_NTVDM_STARTUP_MACHINE_INTERRUPT_V1_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_generic_ud_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Handles only source-admitted real-mode BIOS BOP 02/06 mechanics through
 * copied mantle actions.  It knows no Bochs object, callback or host state. */
int bx_ntvdm_startup_machine_interrupt_v1_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif