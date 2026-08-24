#ifndef RUNTIME_SOFTPC_MACHINE_INTERRUPT_V2_GENERIC_UD_BRIDGE_H
#define RUNTIME_SOFTPC_MACHINE_INTERRUPT_V2_GENERIC_UD_BRIDGE_H

#include "adapter-bop/generic_ud_bridge.h"

/*
 * Adapter-owned ingress for the historical SoftPC BOP 02/06 interrupt
 * package.  The selector is recognized here, never by bx-core or bx-mantle.
 */
int runtime_softpc_machine_interrupt_v2_generic_ud_recognizes(
    const struct runtime_generic_ud_event_v1 *event);
int runtime_softpc_machine_interrupt_v2_generic_ud_dispatch(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome);

#endif
