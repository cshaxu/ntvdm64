#include "softpc_emm_unavailable_v2_generic_ud_bridge.h"
#include "adapter-softpc/softpc_emm_unavailable_shim.h"
#include "cpu_state_abi.h"

#include <string.h>

void emm_init(void);

int runtime_softpc_emm_unavailable_v2_generic_ud_recognizes(
    const struct runtime_generic_ud_event_v1 *event)
{
    return event != 0 && event->magic == RUNTIME_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->execution_mode == RUNTIME_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x66u;
}

int runtime_softpc_emm_unavailable_v2_generic_ud_dispatch(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome)
{
    uint16_t bx;
    if (!runtime_softpc_emm_unavailable_v2_generic_ud_recognizes(event) ||
        outcome == 0 || event->fault_rip > UINT64_MAX - 3u ||
        !runtime_softpc_emm_unavailable_begin(event->ds,
            (uint16_t)event->edx, &bx)) return 0;
    emm_init();
    if (!runtime_softpc_emm_unavailable_end(&bx)) return 0;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = RUNTIME_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 3u;
    outcome->gpr16_write_mask = 2u;
    outcome->gpr16_values[1] = bx;
    return 1;
}
