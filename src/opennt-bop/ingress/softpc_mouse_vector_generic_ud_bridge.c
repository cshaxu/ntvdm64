#include "softpc_mouse_vector_generic_ud_bridge.h"
#include "adapter-softpc/softpc_mouse_vector_shim.h"
#include "cpu_state_abi.h"

#include <string.h>

void host_mouse_install1(void);

int runtime_softpc_mouse_vector_generic_ud_recognizes(
    const struct runtime_generic_ud_event *event)
{
    return event != 0 && event->magic == RUNTIME_GENERIC_UD_EVENT_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->execution_mode == RUNTIME_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0xc8u;
}

int runtime_softpc_mouse_vector_generic_ud_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    if (!runtime_softpc_mouse_vector_generic_ud_recognizes(event) ||
        outcome == 0 || event->fault_rip > UINT64_MAX - 3u ||
        !runtime_softpc_mouse_vector_begin(event->cs,
            (uint16_t)event->ebx)) return 0;
    host_mouse_install1();
    if (!runtime_softpc_mouse_vector_end()) return 0;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    outcome->disposition = RUNTIME_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 3u;
    return 1;
}
