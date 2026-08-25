#include "spckbd_handoff_generic_ud_bridge.h"
#include "adapter-softpc/spckbd_handoff_shim.h"
#include "cpu_state_abi.h"
#include "cpu_result.h"

#include <string.h>

void MS_bop_F(void);

int runtime_spckbd_handoff_generic_ud_recognizes(
    const struct runtime_generic_ud_event *event)
{
    return event != 0 && event->magic == RUNTIME_GENERIC_UD_EVENT_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->execution_mode == RUNTIME_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x5fu &&
        (uint16_t)event->eax == 0xbeefu;
}

int runtime_spckbd_handoff_generic_ud_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    if (!runtime_spckbd_handoff_generic_ud_recognizes(event) ||
        outcome == 0 || event->fault_rip > UINT64_MAX - 3u ||
        !runtime_spckbd_handoff_begin(event->cs, event->ds,
            (uint16_t)event->esi, (uint16_t)event->eax,
            runtime_spckbd_handoff_display_state())) return 0;
    MS_bop_F();
    if (!runtime_spckbd_handoff_end()) return 0;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    outcome->disposition = RUNTIME_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 3u;
    outcome->eflags_write_mask = RUNTIME_CPU_RESULT_EFLAGS_CF;
    outcome->eflags_values = RUNTIME_CPU_RESULT_EFLAGS_CF;
    return 1;
}
