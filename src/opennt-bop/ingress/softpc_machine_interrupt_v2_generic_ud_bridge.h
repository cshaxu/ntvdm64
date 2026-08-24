#ifndef BX_NTVDM_SOFTPC_MACHINE_INTERRUPT_V2_GENERIC_UD_BRIDGE_H
#define BX_NTVDM_SOFTPC_MACHINE_INTERRUPT_V2_GENERIC_UD_BRIDGE_H

#include "adapter-softpc/bx_ntvdm_generic_ud_bridge.h"

/*
 * Adapter-owned ingress for the historical SoftPC BOP 02/06 interrupt
 * package.  The selector is recognized here, never by bx-core or bx-mantle.
 */
int bx_ntvdm_softpc_machine_interrupt_v2_generic_ud_recognizes(
    const struct bx_ntvdm_generic_ud_event_v1 *event);
int bx_ntvdm_softpc_machine_interrupt_v2_generic_ud_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

#endif
