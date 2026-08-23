#include <stdint.h>
#include <string.h>

#include "bop/softpc_emm_unavailable_v2_generic_ud_bridge.h"

int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

static void initialize(struct bx_ntvdm_generic_ud_event_v1 *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = 1u;
    event->fault_rip = UINT64_C(0x79c5);
    event->ds = UINT16_C(0x8dc8);
    event->edx = UINT32_C(0x1234);
    event->eflags = UINT32_C(0x246);
    event->window_bytes = 3u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = 0x66u;
}

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    initialize(&event);
    if (!bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_recognizes(&event) ||
        !bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_dispatch(&event,
            &outcome) || outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != UINT64_C(0x79c8) ||
        outcome.gpr16_write_mask != 2u || outcome.gpr16_values[1] != 0u ||
        outcome.eflags_write_mask != 0u) return 1;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != UINT64_C(0x79c8) ||
        outcome.gpr16_write_mask != 2u || outcome.gpr16_values[1] != 0u ||
        outcome.eflags_write_mask != 0u) return 5;
    event.execution_mode = 2u;
    if (bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_recognizes(&event) ||
        bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_dispatch(&event,
            &outcome)) return 2;
    event.execution_mode = 1u;
    event.window[2] = 0x68u;
    if (bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_recognizes(&event)) return 3;
    event.window[2] = 0x66u;
    event.fault_rip = UINT64_MAX - 2u;
    if (bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_dispatch(&event,
            &outcome)) return 4;
    return 0;
}
