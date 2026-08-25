#ifndef RUNTIME_STARTUP_MACHINE_INTERRUPT_H
#define RUNTIME_STARTUP_MACHINE_INTERRUPT_H

#include "cpu_result.h"
#include "cpu_state_abi.h"
#include "generic_ud_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Handles only source-admitted real-mode BIOS BOP 02/06 mechanics through
 * copied machine actions.  It knows no Bochs object, callback or host state. */
int runtime_startup_machine_interrupt_dispatch(
    const struct runtime_generic_ud_event *event,
    runtime_cpu_result *result);

#ifdef __cplusplus
}
#endif

#endif