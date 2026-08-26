#include <stdint.h>
#include "app/bop/route.h"
#include <string.h>
#include "app/bop/route.h"

#include "opennt-bop/ingress/softpc_emm_unavailable_generic_ud_bridge.h"
#include "app/bop/route.h"

int runtime_machine_generic_ud_bridge(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome);

static void initialize(struct runtime_generic_ud_event *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
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
    if (!runtime_bop_ingress_bind(runtime_opennt_bop_route_dispatch, 0)) return 90;
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    initialize(&event);
    if (!runtime_softpc_emm_unavailable_generic_ud_recognizes(&event) ||
        !runtime_softpc_emm_unavailable_generic_ud_dispatch(&event,
            &outcome) || outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != UINT64_C(0x79c8) ||
        outcome.gpr16_write_mask != 2u || outcome.gpr16_values[1] != 0u ||
        outcome.eflags_write_mask != 0u) return 1;
    if (!runtime_machine_generic_ud_bridge(&event, &outcome) ||
        outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != UINT64_C(0x79c8) ||
        outcome.gpr16_write_mask != 2u || outcome.gpr16_values[1] != 0u ||
        outcome.eflags_write_mask != 0u) return 5;
    event.execution_mode = 2u;
    if (runtime_softpc_emm_unavailable_generic_ud_recognizes(&event) ||
        runtime_softpc_emm_unavailable_generic_ud_dispatch(&event,
            &outcome)) return 2;
    event.execution_mode = 1u;
    event.window[2] = 0x68u;
    if (runtime_softpc_emm_unavailable_generic_ud_recognizes(&event)) return 3;
    event.window[2] = 0x66u;
    event.fault_rip = UINT64_MAX - 2u;
    if (runtime_softpc_emm_unavailable_generic_ud_dispatch(&event,
            &outcome)) return 4;
    return 0;
}
