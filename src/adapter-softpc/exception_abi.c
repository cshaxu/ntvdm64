#include "exception_abi.h"

int runtime_exception_event_v1_valid(const runtime_exception_event_v1 *event)
{
    return event != 0 &&
        event->magic == RUNTIME_EXCEPTION_ABI_MAGIC &&
        event->abi_version == RUNTIME_EXCEPTION_ABI_VERSION &&
        event->struct_bytes == sizeof(*event) &&
        event->kind == RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
}

void runtime_exception_result_v1_pass_through(runtime_exception_result_v1 *result)
{
    if (result == 0) {
        return;
    }
    result->magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    result->abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    result->struct_bytes = sizeof(*result);
    result->disposition = RUNTIME_EXCEPTION_RESULT_PASS_THROUGH;
    result->resume_rip = 0;
    runtime_cpu_delta_v1_initialize(&result->cpu_delta);
}

int runtime_exception_result_v1_resume(runtime_exception_result_v1 *result,
    uint64_t resume_rip)
{
    if (result == 0) {
        return 0;
    }
    runtime_exception_result_v1_pass_through(result);
    result->disposition = RUNTIME_EXCEPTION_RESULT_RESUME;
    result->resume_rip = resume_rip;
    return 1;
}

int runtime_exception_dispatch_v1(const runtime_exception_event_v1 *event,
    runtime_exception_result_v1 *result)
{
    if (!runtime_exception_event_v1_valid(event) || result == 0) {
        return 0;
    }
    runtime_exception_result_v1_pass_through(result);
    return 1;
}

int runtime_exception_dispatch_state_v1(
    const runtime_exception_event_v1 *event,
    const runtime_cpu_state_v1 *state,
    runtime_exception_result_v1 *result)
{
    if (!runtime_exception_event_v1_valid(event) ||
        !runtime_cpu_state_v1_valid(state) || result == 0) {
        return 0;
    }
    runtime_exception_result_v1_pass_through(result);
    return 1;
}

int runtime_exception_dispatch_state_window_v1(
    const runtime_exception_event_v1 *event,
    const runtime_cpu_state_v1 *state,
    const runtime_instruction_window_v1 *window,
    runtime_exception_result_v1 *result)
{
    if (!runtime_instruction_window_v1_valid(window)) return 0;
    return runtime_exception_dispatch_state_v1(event, state, result);
}
