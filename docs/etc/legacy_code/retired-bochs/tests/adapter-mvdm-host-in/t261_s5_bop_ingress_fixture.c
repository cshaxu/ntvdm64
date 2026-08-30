#include "adapter-mvdm-host-in/bop_ingress.h"

#include <string.h>

static int calls;

static int route(const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome, void *context)
{
    if (event == 0 || outcome == 0 || context != &calls ||
        event->window_bytes != 3u || event->window[0] != 0xc4u) return 0;
    ++calls;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    outcome->disposition = RUNTIME_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 3u;
    return 1;
}

int main(void)
{
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    memset(&event, 0, sizeof(event));
    event.magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event.abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event.struct_bytes = sizeof(event); event.vector = 6u;
    event.fault_rip = 0x1000u; event.window_bytes = 3u;
    event.window[0] = 0xc4u; event.window[1] = 0xc4u; event.window[2] = 0x50u;
    if (runtime_bop_ingress_bound() ||
        runtime_machine_generic_ud_bridge(&event, &outcome)) return 1;
    if (!runtime_bop_ingress_bind(route, &calls) ||
        runtime_bop_ingress_bind(route, &calls) ||
        !runtime_bop_ingress_bound()) return 2;
    if (!runtime_machine_generic_ud_bridge(&event, &outcome) || calls != 1 ||
        outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x1003u) return 3;
    memset(&outcome, 0, sizeof(outcome));
    if (!runtime_bop_ingress_opaque_callback(0, &event, sizeof(event),
            &outcome, sizeof(outcome)) || calls != 2 ||
        outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x1003u) return 5;
    runtime_bop_ingress_unbind();
    return !runtime_bop_ingress_bound() &&
        !runtime_machine_generic_ud_bridge(&event, &outcome) ? 0 : 4;
}
