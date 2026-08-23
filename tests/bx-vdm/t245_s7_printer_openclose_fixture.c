#include <stdint.h>
#include <string.h>

#include "bop/softpc_printer_openclose_v2_generic_ud_bridge.h"
#include "bop/shim/softpc_printer_openclose_shim.h"

int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

static void initialize(struct bx_ntvdm_generic_ud_event_v1 *event,
    uint16_t si, uint16_t dx)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
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
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    uint32_t opened = 0u;

    bx_ntvdm_softpc_printer_openclose_reset();
    initialize(&event, 1u, 4u);
    if (!bx_ntvdm_softpc_printer_openclose_v2_generic_ud_recognizes(&event) ||
        !bx_ntvdm_softpc_printer_openclose_v2_generic_ud_dispatch(&event,
            &outcome) || outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != UINT64_C(0x02d8) ||
        outcome.gpr16_write_mask != 0u || outcome.eflags_write_mask != 0u ||
        !bx_ntvdm_softpc_printer_dos_open_copy(1u, &opened) || opened != 1u)
        return 1;
    initialize(&event, 2u, 4u);
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        !bx_ntvdm_softpc_printer_dos_open_copy(1u, &opened) || opened != 0u)
        return 2;
    initialize(&event, 0u, 0u);
    if (!bx_ntvdm_softpc_printer_openclose_v2_generic_ud_recognizes(&event) ||
        bx_ntvdm_softpc_printer_openclose_v2_generic_ud_dispatch(&event,
            &outcome) || bx_ntvdm_mantle_generic_ud_bridge_v1(&event,
                &outcome)) return 3;
    event.execution_mode = 2u;
    if (bx_ntvdm_softpc_printer_openclose_v2_generic_ud_recognizes(&event))
        return 4;
    event.execution_mode = 1u;
    event.esi = 1u;
    event.fault_rip = UINT64_MAX - 2u;
    if (bx_ntvdm_softpc_printer_openclose_v2_generic_ud_dispatch(&event,
            &outcome)) return 5;
    return 0;
}
