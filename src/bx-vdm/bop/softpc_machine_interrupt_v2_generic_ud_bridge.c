#include "softpc_machine_interrupt_v2_generic_ud_bridge.h"

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_startup_machine_interrupt_v1.h"

#include <string.h>

int bx_ntvdm_softpc_machine_interrupt_v2_generic_ud_recognizes(
    const struct bx_ntvdm_generic_ud_event_v1 *event)
{
    return event != NULL && event->magic == BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u &&
        (event->window[2] == 0x02u || event->window[2] == 0x06u);
}

static int copy_outcome(const bx_ntvdm_cpu_result_v2 *result,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (!bx_ntvdm_cpu_result_v2_valid(result) || outcome == NULL ||
        result->disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH) return 0;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME ?
        BX_NTVDM_GENERIC_UD_RESUME : BX_NTVDM_GENERIC_UD_STOP;
    outcome->resume_rip = result->resume_rip;
    outcome->gpr16_write_mask = result->cpu_delta.gpr16_write_mask;
    memcpy(outcome->gpr16_values, result->cpu_delta.gpr16_values,
        sizeof(outcome->gpr16_values));
    outcome->segment_write_mask = result->cpu_delta.segment_write_mask;
    memcpy(outcome->segment_values, result->cpu_delta.segment_values,
        sizeof(outcome->segment_values));
    outcome->eflags_write_mask = result->eflags_write_mask;
    outcome->eflags_values = result->eflags_values;
    return 1;
}

int bx_ntvdm_softpc_machine_interrupt_v2_generic_ud_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    bx_ntvdm_cpu_result_v2 result;
    if (!bx_ntvdm_softpc_machine_interrupt_v2_generic_ud_recognizes(event) ||
        outcome == NULL) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    return bx_ntvdm_startup_machine_interrupt_v1_dispatch(event, &result) &&
        copy_outcome(&result, outcome);
}
