#include <stdio.h>
#include <string.h>

#include "bop/softpc_tape_io_v2_generic_ud_bridge.h"
#include "bx_ntvdm_cpu_state_abi.h"

static void initialize(struct bx_ntvdm_generic_ud_event_v1 *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event->fault_rip = 0x655u;
    event->eax = 0x88a7u;
    event->window_bytes = 3u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = 0x15u;
}

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    initialize(&event);
    if (!bx_ntvdm_softpc_tape_io_v2_generic_ud_recognizes(&event) ||
        !bx_ntvdm_softpc_tape_io_v2_generic_ud_dispatch(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x658u || outcome.gpr16_write_mask != 1u ||
        outcome.gpr16_values[0] != 0u || outcome.eflags_write_mask != 0u)
        return 1;
    event.eax = 0x8700u;
    if (bx_ntvdm_softpc_tape_io_v2_generic_ud_recognizes(&event) ||
        bx_ntvdm_softpc_tape_io_v2_generic_ud_dispatch(&event, &outcome))
        return 2;
    event.eax = 0x8800u;
    event.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    if (bx_ntvdm_softpc_tape_io_v2_generic_ud_recognizes(&event)) return 3;
    puts("T242 S3 source-shaped SoftPC INT15 AH=88h non-PM route passes");
    return 0;
}
