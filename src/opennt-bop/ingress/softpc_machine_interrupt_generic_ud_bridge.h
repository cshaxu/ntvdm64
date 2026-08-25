#ifndef RUNTIME_SOFTPC_MACHINE_INTERRUPT_GENERIC_UD_BRIDGE_H
#define RUNTIME_SOFTPC_MACHINE_INTERRUPT_GENERIC_UD_BRIDGE_H

#include "adapter-bop/generic_ud_bridge.h"

/*
 * Adapter-owned ingress for the historical SoftPC BOP 02/06 interrupt
 * package.  The selector is recognized here, never by bochs-core or adapter-bochs.
 */
int runtime_softpc_machine_interrupt_generic_ud_recognizes(
    const struct runtime_generic_ud_event *event);
int runtime_softpc_machine_interrupt_generic_ud_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome);

#endif
