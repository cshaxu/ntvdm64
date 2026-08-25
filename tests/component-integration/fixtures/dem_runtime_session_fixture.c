#include <stdio.h>
#include <string.h>

#include "opennt-bop/ingress/dem_runtime_session.h"
#include "adapter-bop/generic_ud_bridge.h"
#include "opennt-bop/ingress/opennt_bop_route.h"

static void event_initialize(struct runtime_generic_ud_event *event,
    uint8_t selector, uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = 1u;
    event->fault_rip = 0x2600u;
    event->eip = 0x2600u;
    event->window_bytes = 4u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = selector;
    event->window[3] = service;
}

int main(void)
{
    if (!runtime_bop_ingress_bind(runtime_opennt_bop_route_dispatch, 0)) return 90;
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;

    if (!runtime_dem_runtime_session_bind()) return 1;
    event_initialize(&event, 0x50u, 0x1fu);
    memset(&outcome, 0, sizeof(outcome));
    if (!runtime_machine_generic_ud_bridge(&event, &outcome) ||
        outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x2604u) return 2;
    event_initialize(&event, 0x54u, 0x1fu);
    if (runtime_machine_generic_ud_bridge(&event, &outcome)) return 3;
    runtime_dem_runtime_session_reset();
    event_initialize(&event, 0x50u, 0x1fu);
    if (runtime_machine_generic_ud_bridge(&event, &outcome)) return 4;
    puts("T230 v2 runtime session owns DEM and declines without legacy fallback");
    return 0;
}