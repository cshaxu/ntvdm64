#include "adapter-softpc/machine_stage.h"
#include "app/bop/route.h"
#include "adapter-softpc/mechanical_action.h"
#include "app/bop/route.h"
#include "opennt-bop/ingress/softpc_mouse_vector_generic_ud_bridge.h"
#include "app/bop/route.h"

#include <stdint.h>
#include "app/bop/route.h"
#include <string.h>
#include "app/bop/route.h"

extern "C" int runtime_machine_generic_ud_bridge(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome);

static void write_action(struct runtime_mechanical_action *action,
    uint32_t id, uint64_t address, uint8_t value)
{
    runtime_mechanical_action_clear(action);
    action->action_id = id;
    action->kind = RUNTIME_MECHANICAL_ACTION_WRITE;
    action->range_count = 1u;
    action->payload_bytes = 1u;
    action->ranges[0].physical_address = address;
    action->ranges[0].byte_count = 1u;
    action->payload[0] = value;
}

static int begin_stage(void)
{
    struct runtime_machine_stage_request request;
    runtime_machine_stage_request_clear(&request);
    write_action(&request.initial_state_action, 1u, 0x40u, 0u);
    write_action(&request.startup_action, 2u, 0x100u, 0xf4u);
    request.preserved_state_address = 0x40u;
    request.preserved_state_bytes = 1u;
    return runtime_machine_stage_begin(&request) ==
        RUNTIME_MACHINE_STAGE_OK;
}

static void initialize(struct runtime_generic_ud_event *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = 1u;
    event->fault_rip = UINT64_C(0x78cb);
    event->cs = UINT16_C(0x0200);
    event->ebx = UINT32_C(0x0010);
    event->eflags = UINT32_C(0x246);
    event->window_bytes = 3u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = 0xc8u;
}

int main()
{
    if (!runtime_bop_ingress_bind(runtime_opennt_bop_route_dispatch, 0)) return 90;
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    const uint8_t table[4] = { 0x34u, 0x12u, 0x78u, 0x56u };
    uint8_t observed[4] = { 0u, 0u, 0u, 0u };

    if (!begin_stage() || !runtime_machine_checked_ram_write(0x2010u,
            table, sizeof(table))) return 1;
    initialize(&event);
    if (!runtime_softpc_mouse_vector_generic_ud_recognizes(&event) ||
        !runtime_softpc_mouse_vector_generic_ud_dispatch(&event,
            &outcome) || outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != UINT64_C(0x78ce) ||
        outcome.gpr16_write_mask != 0u || outcome.eflags_write_mask != 0u ||
        !runtime_machine_checked_ram_read(0xccu, observed,
            sizeof(observed)) || memcmp(observed, table, sizeof(table)) != 0)
        return 2;
    memset(observed, 0, sizeof(observed));
    if (!runtime_machine_checked_ram_write(0xccu, observed,
            sizeof(observed)) || !runtime_machine_generic_ud_bridge(&event,
            &outcome) || outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        !runtime_machine_checked_ram_read(0xccu, observed,
            sizeof(observed)) || memcmp(observed, table, sizeof(table)) != 0)
        return 3;
    event.window[2] = 0xc9u;
    if (runtime_softpc_mouse_vector_generic_ud_recognizes(&event) ||
        runtime_machine_generic_ud_bridge(&event, &outcome)) return 4;
    event.window[2] = 0xb8u;
    if (runtime_softpc_mouse_vector_generic_ud_recognizes(&event) ||
        runtime_machine_generic_ud_bridge(&event, &outcome)) return 5;
    event.window[2] = 0xc8u;
    event.cs = UINT16_C(0xffff);
    event.ebx = UINT32_C(0xffff);
    if (runtime_softpc_mouse_vector_generic_ud_dispatch(&event,
            &outcome)) return 6;
    return runtime_machine_stage_reset() == RUNTIME_MACHINE_STAGE_OK ?
        0 : 7;
}
