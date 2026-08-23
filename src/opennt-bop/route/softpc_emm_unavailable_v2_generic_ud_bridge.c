#include "softpc_emm_unavailable_v2_generic_ud_bridge.h"
#include "shim/softpc_emm_unavailable_shim.h"
#include "bx_ntvdm_cpu_state_abi.h"

#include <string.h>

void emm_init(void);

int bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_recognizes(
    const struct bx_ntvdm_generic_ud_event_v1 *event)
{
    return event != 0 && event->magic == BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x66u;
}

int bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    uint16_t bx;
    if (!bx_ntvdm_softpc_emm_unavailable_v2_generic_ud_recognizes(event) ||
        outcome == 0 || event->fault_rip > UINT64_MAX - 3u ||
        !bx_ntvdm_softpc_emm_unavailable_begin(event->ds,
            (uint16_t)event->edx, &bx)) return 0;
    emm_init();
    if (!bx_ntvdm_softpc_emm_unavailable_end(&bx)) return 0;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = BX_NTVDM_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 3u;
    outcome->gpr16_write_mask = 2u;
    outcome->gpr16_values[1] = bx;
    return 1;
}
