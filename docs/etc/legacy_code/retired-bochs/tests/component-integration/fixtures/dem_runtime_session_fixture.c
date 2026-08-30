#include <stdio.h>
#include <string.h>

#include "app/bop/dem_runtime.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"
#include "adapter-mvdm-host-out/softpc/machine_stage.h"
#include "adapter-mvdm-host-out/softpc/mechanical_action.h"
#include "app/bop/route.h"

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
    struct runtime_machine_stage_request machine;
    if (!runtime_bop_ingress_bind(runtime_opennt_bop_route_dispatch, 0)) return 90;
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;

    runtime_machine_stage_request_clear(&machine);
    runtime_mechanical_action_clear(&machine.initial_state_action);
    machine.initial_state_action.action_id = 1u;
    machine.initial_state_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
    machine.initial_state_action.range_count = 1u;
    machine.initial_state_action.payload_bytes = 1u;
    machine.initial_state_action.ranges[0].physical_address = 0x40u;
    machine.initial_state_action.ranges[0].byte_count = 1u;
    machine.initial_state_action.payload[0] = 0u;
    runtime_mechanical_action_clear(&machine.startup_action);
    machine.startup_action.action_id = 2u;
    machine.startup_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
    machine.startup_action.range_count = 1u;
    machine.startup_action.payload_bytes = 1u;
    machine.startup_action.ranges[0].physical_address = 0x100u;
    machine.startup_action.ranges[0].byte_count = 1u;
    machine.startup_action.payload[0] = 0xf4u;
    machine.preserved_state_address = 0x40u;
    machine.preserved_state_bytes = 1u;
    if (runtime_machine_stage_begin(&machine) != RUNTIME_MACHINE_STAGE_OK) return 1;
    {
        const uint8_t bop[] = { 0xc4u, 0xc4u, 0x50u, 0x1fu };
        if (!runtime_machine_checked_ram_write(0x2600u, bop, sizeof(bop))) return 2;
    }
    if (!runtime_dem_runtime_session_bind()) return 3;
    event_initialize(&event, 0x50u, 0x1fu);
    memset(&outcome, 0, sizeof(outcome));
    if (!runtime_machine_generic_ud_bridge(&event, &outcome) ||
        outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x2604u) return 4;
    event_initialize(&event, 0x54u, 0x1fu);
    if (runtime_machine_generic_ud_bridge(&event, &outcome)) return 5;
    runtime_dem_runtime_session_reset();
    event_initialize(&event, 0x50u, 0x1fu);
    if (runtime_machine_generic_ud_bridge(&event, &outcome)) return 6;
    if (runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK) return 7;
    puts("T230 v2 runtime session owns DEM and declines without legacy fallback");
    return 0;
}
