#include <stdio.h>
#include <string.h>

#include "app/bop/command_entry.h"
#include "app/bop/command_session.h"
#include "adapter-softpc/machine_stage.h"
#include "adapter-softpc/mechanical_action.h"

static void event_initialize(struct runtime_generic_ud_event *event,
    uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = RUNTIME_CPU_EXECUTION_REAL;
    event->fault_rip = 0x2800u;
    event->eip = 0x2800u;
    event->window_bytes = 4u;
    event->window[0] = 0xc4u; event->window[1] = 0xc4u;
    event->window[2] = 0x54u; event->window[3] = service;
}

int main(void)
{
    runtime_command_native_session session;
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    struct runtime_machine_stage_request machine;

    runtime_machine_stage_request_clear(&machine);
    runtime_mechanical_action_clear(&machine.initial_state_action);
    machine.initial_state_action.action_id = 1u;
    machine.initial_state_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
    machine.initial_state_action.range_count = 1u;
    machine.initial_state_action.payload_bytes = 1u;
    machine.initial_state_action.ranges[0].physical_address = 0x40u;
    machine.initial_state_action.ranges[0].byte_count = 1u;
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
        const uint8_t bop[] = { 0xc4u, 0xc4u, 0x54u, 0u };
        if (!runtime_machine_checked_ram_write(0x2800u, bop, sizeof(bop))) return 2;
    }
    if (!runtime_command_native_session_initialize(&session) ||
        !runtime_command_native_session_bind(&session)) return 3;
    event_initialize(&event, 0u);
    memset(&outcome, 0, sizeof(outcome));
    if (!runtime_command_generic_ud_dispatch(&event, &outcome) ||
        outcome.disposition != RUNTIME_GENERIC_UD_STOP ||
        outcome.resume_rip != 0u) return 4;
    event_initialize(&event, 17u);
    if (runtime_command_generic_ud_recognizes(&event) ||
        runtime_command_generic_ud_dispatch(&event, &outcome)) return 5;
    runtime_command_native_session_unbind(&session);
    event_initialize(&event, 0u);
    if (runtime_command_generic_ud_dispatch(&event, &outcome)) return 6;
    if (runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK) return 7;
    puts("T231 v2 COMMAND session owns 54:00 without a v1 fallback");
    return 0;
}
