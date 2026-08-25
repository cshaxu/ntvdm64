#include <stdio.h>
#include <string.h>

#include "opennt-bop/ingress/softpc_tape_io_generic_ud_bridge.h"
#include "cpu_state_abi.h"

static void initialize(struct runtime_generic_ud_event *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = RUNTIME_CPU_EXECUTION_REAL;
    event->fault_rip = 0x655u;
    event->eax = 0x88a7u;
    event->window_bytes = 3u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = 0x15u;
}

int main(void)
{
    struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    initialize(&event);
    if (!runtime_softpc_tape_io_generic_ud_recognizes(&event) ||
        !runtime_softpc_tape_io_generic_ud_dispatch(&event, &outcome) ||
        outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x658u || outcome.gpr16_write_mask != 1u ||
        outcome.gpr16_values[0] != 0u || outcome.eflags_write_mask != 0u)
        return 1;
    event.eax = 0x8700u;
    if (runtime_softpc_tape_io_generic_ud_recognizes(&event) ||
        runtime_softpc_tape_io_generic_ud_dispatch(&event, &outcome))
        return 2;
    event.eax = 0x8800u;
    event.execution_mode = RUNTIME_CPU_EXECUTION_PROTECTED;
    if (runtime_softpc_tape_io_generic_ud_recognizes(&event)) return 3;
    puts("T242 S3 source-shaped SoftPC INT15 AH=88h non-PM route passes");
    return 0;
}
