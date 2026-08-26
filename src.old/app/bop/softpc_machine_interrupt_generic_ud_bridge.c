#include "softpc_machine_interrupt_generic_ud_bridge.h"

#include "cpu_result.h"
#include "startup_machine_interrupt.h"

#include <string.h>

int runtime_softpc_machine_interrupt_generic_ud_recognizes(
    const struct runtime_generic_ud_event *event)
{
    return event != NULL && event->magic == RUNTIME_GENERIC_UD_EVENT_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->execution_mode == RUNTIME_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u &&
        (event->window[2] == 0x02u || event->window[2] == 0x06u);
}

static int copy_outcome(const runtime_cpu_result *result,
    struct runtime_generic_ud_outcome *outcome)
{
    if (!runtime_cpu_result_valid(result) || outcome == NULL ||
        result->disposition == RUNTIME_CPU_RESULT_PASS_THROUGH) return 0;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    outcome->disposition = result->disposition == RUNTIME_CPU_RESULT_RESUME ?
        RUNTIME_GENERIC_UD_RESUME : RUNTIME_GENERIC_UD_STOP;
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

int runtime_softpc_machine_interrupt_generic_ud_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    runtime_cpu_result result;
    if (!runtime_softpc_machine_interrupt_generic_ud_recognizes(event) ||
        outcome == NULL) return 0;
    runtime_cpu_result_pass_through(&result);
    return runtime_startup_machine_interrupt_dispatch(event, &result) &&
        copy_outcome(&result, outcome);
}
