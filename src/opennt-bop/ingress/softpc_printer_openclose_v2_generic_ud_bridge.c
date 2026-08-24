#include "softpc_printer_openclose_v2_generic_ud_bridge.h"
#include "adapter-softpc/softpc_printer_openclose_shim.h"
#include "cpu_state_abi.h"

#include <string.h>

void printer_io(void);

int runtime_softpc_printer_openclose_v2_generic_ud_recognizes(
    const struct runtime_generic_ud_event_v1 *event)
{
    return event != 0 && event->magic == RUNTIME_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->execution_mode == RUNTIME_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x17u;
}

int runtime_softpc_printer_openclose_v2_generic_ud_dispatch(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome)
{
    if (!runtime_softpc_printer_openclose_v2_generic_ud_recognizes(event) ||
        outcome == 0 || event->fault_rip > UINT64_MAX - 3u ||
        !runtime_softpc_printer_openclose_begin((uint16_t)event->esi,
            (uint16_t)event->edx)) return 0;
    printer_io();
    if (!runtime_softpc_printer_openclose_end()) return 0;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = RUNTIME_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 3u;
    return 1;
}
