#include "bx_ntvdm_exception_abi.h"

int bx_ntvdm_exception_event_v1_valid(const bx_ntvdm_exception_event_v1 *event)
{
    return event != 0 &&
        event->magic == BX_NTVDM_EXCEPTION_ABI_MAGIC &&
        event->abi_version == BX_NTVDM_EXCEPTION_ABI_VERSION &&
        event->struct_bytes == sizeof(*event) &&
        event->kind == BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
}

void bx_ntvdm_exception_result_v1_pass_through(bx_ntvdm_exception_result_v1 *result)
{
    if (result == 0) {
        return;
    }
    result->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    result->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    result->struct_bytes = sizeof(*result);
    result->disposition = BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH;
    result->resume_rip = 0;
    bx_ntvdm_cpu_delta_v1_initialize(&result->cpu_delta);
}

int bx_ntvdm_exception_result_v1_resume(bx_ntvdm_exception_result_v1 *result,
    uint64_t resume_rip)
{
    if (result == 0) {
        return 0;
    }
    bx_ntvdm_exception_result_v1_pass_through(result);
    result->disposition = BX_NTVDM_EXCEPTION_RESULT_RESUME;
    result->resume_rip = resume_rip;
    return 1;
}

int bx_ntvdm_exception_dispatch_v1(const bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_exception_result_v1 *result)
{
    if (!bx_ntvdm_exception_event_v1_valid(event) || result == 0) {
        return 0;
    }
    bx_ntvdm_exception_result_v1_pass_through(result);
    return 1;
}

int bx_ntvdm_exception_dispatch_state_v1(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *state,
    bx_ntvdm_exception_result_v1 *result)
{
    if (!bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(state) || result == 0) {
        return 0;
    }
    bx_ntvdm_exception_result_v1_pass_through(result);
    return 1;
}

int bx_ntvdm_exception_dispatch_state_window_v1(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *state,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result)
{
    if (!bx_ntvdm_instruction_window_v1_valid(window)) return 0;
    return bx_ntvdm_exception_dispatch_state_v1(event, state, result);
}
