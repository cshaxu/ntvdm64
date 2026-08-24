#ifndef RUNTIME_STARTUP_MACHINE_INTERRUPT_V1_H
#define RUNTIME_STARTUP_MACHINE_INTERRUPT_V1_H

#include "cpu_result.h"
#include "cpu_state_abi.h"
#include "generic_ud_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Handles only source-admitted real-mode BIOS BOP 02/06 mechanics through
 * copied mantle actions.  It knows no Bochs object, callback or host state. */
int runtime_startup_machine_interrupt_v1_dispatch(
    const struct runtime_generic_ud_event_v1 *event,
    runtime_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif