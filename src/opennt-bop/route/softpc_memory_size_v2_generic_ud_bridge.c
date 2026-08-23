#include "softpc_memory_size_v2_generic_ud_bridge.h"
#include "shim/softpc_memory_size_shim.h"
#include "bx_ntvdm_cpu_state_abi.h"

#include <string.h>

/* Direct source mirror entry in opennt/softpc/mem_size.c. */
void memory_size(void);

int bx_ntvdm_softpc_memory_size_v2_generic_ud_recognizes(
    const struct bx_ntvdm_generic_ud_event_v1 *event)
{
    return event != 0 && event->magic == BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x12u;
}

int bx_ntvdm_softpc_memory_size_v2_generic_ud_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    uint16_t ax;
    if (!bx_ntvdm_softpc_memory_size_v2_generic_ud_recognizes(event) ||
        outcome == 0 || event->fault_rip > UINT64_MAX - 3u ||
        !bx_ntvdm_softpc_memory_size_begin(&ax)) return 0;
    memory_size();
    if (!bx_ntvdm_softpc_memory_size_end(&ax)) return 0;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = BX_NTVDM_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 3u;
    outcome->gpr16_write_mask = 1u;
    outcome->gpr16_values[0] = ax;
    return 1;
}
