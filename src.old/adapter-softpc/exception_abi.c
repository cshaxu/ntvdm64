#include "exception_abi.h"

int runtime_exception_event_valid(const runtime_exception_event *event)
{
    return event != 0 &&
        event->magic == RUNTIME_EXCEPTION_ABI_MAGIC &&
        event->abi_version == RUNTIME_EXCEPTION_ABI_VERSION &&
        event->struct_bytes == sizeof(*event) &&
        event->kind == RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
}

void runtime_exception_result_pass_through(runtime_exception_result *result)
{
    if (result == 0) {
        return;
    }
    result->magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    result->abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    result->struct_bytes = sizeof(*result);
    result->disposition = RUNTIME_EXCEPTION_RESULT_PASS_THROUGH;
    result->resume_rip = 0;
    runtime_cpu_delta_initialize(&result->cpu_delta);
}

int runtime_exception_result_resume(runtime_exception_result *result,
    uint64_t resume_rip)
{
    if (result == 0) {
        return 0;
    }
    runtime_exception_result_pass_through(result);
    result->disposition = RUNTIME_EXCEPTION_RESULT_RESUME;
    result->resume_rip = resume_rip;
    return 1;
}

int runtime_exception_dispatch(const runtime_exception_event *event,
    runtime_exception_result *result)
{
    if (!runtime_exception_event_valid(event) || result == 0) {
        return 0;
    }
    runtime_exception_result_pass_through(result);
    return 1;
}

int runtime_exception_dispatch_state(
    const runtime_exception_event *event,
    const runtime_cpu_state *state,
    runtime_exception_result *result)
{
    if (!runtime_exception_event_valid(event) ||
        !runtime_cpu_state_valid(state) || result == 0) {
        return 0;
    }
    runtime_exception_result_pass_through(result);
    return 1;
}

int runtime_exception_dispatch_state_window(
    const runtime_exception_event *event,
    const runtime_cpu_state *state,
    const runtime_instruction_window *window,
    runtime_exception_result *result)
{
    if (!runtime_instruction_window_valid(window)) return 0;
    return runtime_exception_dispatch_state(event, state, result);
}
