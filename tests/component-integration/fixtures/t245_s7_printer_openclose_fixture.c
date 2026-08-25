#include <stdint.h>
#include <string.h>

#include "opennt-bop/ingress/softpc_printer_openclose_generic_ud_bridge.h"
#include "opennt-bop/ingress/opennt_bop_route.h"
#include "adapter-softpc/softpc_printer_openclose_shim.h"

int runtime_machine_generic_ud_bridge(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome);

static void initialize(struct runtime_generic_ud_event *event,
    uint16_t si, uint16_t dx)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = 1u;
    event->fault_rip = UINT64_C(0x02d5);
    event->esi = si;
    event->edx = dx;
    event->window_bytes = 3u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = 0x17u;
}

int main(void)
{
    if (!runtime_bop_ingress_bind(runtime_opennt_bop_route_dispatch, 0)) return 90;
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    uint32_t opened = 0u;

    runtime_softpc_printer_openclose_reset();
    initialize(&event, 1u, 4u);
    if (!runtime_softpc_printer_openclose_generic_ud_recognizes(&event) ||
        !runtime_softpc_printer_openclose_generic_ud_dispatch(&event,
            &outcome) || outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != UINT64_C(0x02d8) ||
        outcome.gpr16_write_mask != 0u || outcome.eflags_write_mask != 0u ||
        !runtime_softpc_printer_dos_open_copy(1u, &opened) || opened != 1u)
        return 1;
    initialize(&event, 2u, 4u);
    if (!runtime_machine_generic_ud_bridge(&event, &outcome) ||
        !runtime_softpc_printer_dos_open_copy(1u, &opened) || opened != 0u)
        return 2;
    initialize(&event, 0u, 0u);
    if (!runtime_softpc_printer_openclose_generic_ud_recognizes(&event) ||
        runtime_softpc_printer_openclose_generic_ud_dispatch(&event,
            &outcome) || runtime_machine_generic_ud_bridge(&event,
                &outcome)) return 3;
    event.execution_mode = 2u;
    if (runtime_softpc_printer_openclose_generic_ud_recognizes(&event))
        return 4;
    event.execution_mode = 1u;
    event.esi = 1u;
    event.fault_rip = UINT64_MAX - 2u;
    if (runtime_softpc_printer_openclose_generic_ud_dispatch(&event,
            &outcome)) return 5;
    if (!runtime_softpc_printer_openclose_begin(0u, 0u)) return 6;
    runtime_softpc_printer_host_lpt_dos_open(0);
    runtime_softpc_printer_host_lpt_dos_open(2);
    host_lpt_flush_initialize();
    if (!runtime_softpc_printer_dos_open_copy(0u, &opened) || opened != 0u ||
        !runtime_softpc_printer_dos_open_copy(2u, &opened) || opened != 0u ||
        !runtime_softpc_printer_openclose_end()) return 7;
    return 0;
}
