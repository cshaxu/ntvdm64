#include "adapter-softpc/machine_stage.h"
#include "opennt-bop/ingress/opennt_bop_route.h"
#include "adapter-softpc/mechanical_action.h"
#include "opennt-bop/ingress/opennt_bop_route.h"
#include "opennt-bop/ingress/softpc_mouse_vector_v2_generic_ud_bridge.h"
#include "opennt-bop/ingress/opennt_bop_route.h"

#include <stdint.h>
#include "opennt-bop/ingress/opennt_bop_route.h"
#include <string.h>
#include "opennt-bop/ingress/opennt_bop_route.h"

extern "C" int runtime_mantle_generic_ud_bridge_v1(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome);

static void write_action(struct runtime_mechanical_action_v1 *action,
    uint32_t id, uint64_t address, uint8_t value)
{
    runtime_mechanical_action_v1_clear(action);
    action->action_id = id;
    action->kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
    action->range_count = 1u;
    action->payload_bytes = 1u;
    action->ranges[0].physical_address = address;
    action->ranges[0].byte_count = 1u;
    action->payload[0] = value;
}

static int begin_stage(void)
{
    struct runtime_machine_stage_v1_request request;
    runtime_machine_stage_v1_request_clear(&request);
    write_action(&request.initial_state_action, 1u, 0x40u, 0u);
    write_action(&request.startup_action, 2u, 0x100u, 0xf4u);
    request.preserved_state_address = 0x40u;
    request.preserved_state_bytes = 1u;
    return runtime_machine_stage_v1_begin(&request) ==
        RUNTIME_MACHINE_STAGE_V1_OK;
}

static void initialize(struct runtime_generic_ud_event_v1 *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = RUNTIME_GENERIC_UD_EVENT_V1_VERSION;
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
    if (!runtime_bop_ingress_v1_bind(runtime_opennt_bop_route_dispatch_v1, 0)) return 90;
    struct runtime_generic_ud_event_v1 event;
    struct runtime_generic_ud_outcome_v1 outcome;
    const uint8_t table[4] = { 0x34u, 0x12u, 0x78u, 0x56u };
    uint8_t observed[4] = { 0u, 0u, 0u, 0u };

    if (!begin_stage() || !runtime_mantle_checked_ram_write_v1(0x2010u,
            table, sizeof(table))) return 1;
    initialize(&event);
    if (!runtime_softpc_mouse_vector_v2_generic_ud_recognizes(&event) ||
        !runtime_softpc_mouse_vector_v2_generic_ud_dispatch(&event,
            &outcome) || outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != UINT64_C(0x78ce) ||
        outcome.gpr16_write_mask != 0u || outcome.eflags_write_mask != 0u ||
        !runtime_mantle_checked_ram_read_v1(0xccu, observed,
            sizeof(observed)) || memcmp(observed, table, sizeof(table)) != 0)
        return 2;
    memset(observed, 0, sizeof(observed));
    if (!runtime_mantle_checked_ram_write_v1(0xccu, observed,
            sizeof(observed)) || !runtime_mantle_generic_ud_bridge_v1(&event,
            &outcome) || outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        !runtime_mantle_checked_ram_read_v1(0xccu, observed,
            sizeof(observed)) || memcmp(observed, table, sizeof(table)) != 0)
        return 3;
    event.window[2] = 0xc9u;
    if (runtime_softpc_mouse_vector_v2_generic_ud_recognizes(&event) ||
        runtime_mantle_generic_ud_bridge_v1(&event, &outcome)) return 4;
    event.window[2] = 0xb8u;
    if (runtime_softpc_mouse_vector_v2_generic_ud_recognizes(&event) ||
        runtime_mantle_generic_ud_bridge_v1(&event, &outcome)) return 5;
    event.window[2] = 0xc8u;
    event.cs = UINT16_C(0xffff);
    event.ebx = UINT32_C(0xffff);
    if (runtime_softpc_mouse_vector_v2_generic_ud_dispatch(&event,
            &outcome)) return 6;
    return runtime_machine_stage_v1_reset() == RUNTIME_MACHINE_STAGE_V1_OK ?
        0 : 7;
}