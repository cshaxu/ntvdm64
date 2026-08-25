#include <stdint.h>
#include <string.h>

#include "exception_abi.h"

int main(void)
{
    runtime_exception_event event;
    runtime_exception_result result;
    runtime_cpu_state state;
    runtime_instruction_window window;

    memset(&event, 0, sizeof(event));
    memset(&result, 0, sizeof(result));
    event.magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    event.abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    event.struct_bytes = (uint32_t)sizeof(event);
    event.kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.cpu_id = 0u;
    event.vector = 6u;
    event.error_code = 0u;
    event.fault_rip = UINT64_C(0x00000000000ffff0);

    if (sizeof(event) != 40u || event.struct_bytes != 40u ||
        event.magic != RUNTIME_EXCEPTION_ABI_MAGIC || event.vector != 6u ||
        event.fault_rip != UINT64_C(0x00000000000ffff0)) {
        return 1;
    }

    if (!runtime_exception_event_valid(&event) ||
        runtime_exception_event_valid(0)) {
        return 2;
    }
    if (!runtime_exception_result_resume(&result, event.fault_rip + 2u) ||
        runtime_exception_result_resume(0, event.fault_rip)) {
        return 3;
    }
    if (sizeof(result) != 32u || result.struct_bytes != 32u ||
        result.disposition != RUNTIME_EXCEPTION_RESULT_RESUME ||
        result.resume_rip != UINT64_C(0x00000000000ffff2)) {
        return 4;
    }
    event.kind = 0u;
    if (runtime_exception_event_valid(&event)) { return 5; }
    event.kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
    runtime_exception_result_pass_through(&result);
    if (result.disposition != RUNTIME_EXCEPTION_RESULT_PASS_THROUGH ||
        result.resume_rip != 0u) { return 6; }
    if (!runtime_exception_dispatch(&event, &result) ||
        result.disposition != RUNTIME_EXCEPTION_RESULT_PASS_THROUGH ||
        runtime_exception_dispatch(0, &result)) { return 7; }
    runtime_cpu_state_initialize(&state, RUNTIME_CPU_EXECUTION_REAL);
    if (!runtime_exception_dispatch_state(&event, &state, &result) ||
        result.disposition != RUNTIME_EXCEPTION_RESULT_PASS_THROUGH) { return 8; }
    state.execution_mode = 0u;
    if (runtime_exception_dispatch_state(&event, &state, &result)) { return 9; }
    runtime_cpu_state_initialize(&state, RUNTIME_CPU_EXECUTION_REAL);
    runtime_instruction_window_capture(&window, 0, 0u);
    if (!runtime_exception_dispatch_state_window(&event, &state, &window,
        &result) || result.disposition != RUNTIME_EXCEPTION_RESULT_PASS_THROUGH)
        return 10;
    window.reserved0 = 1u;
    if (runtime_exception_dispatch_state_window(&event, &state, &window,
        &result)) return 11;
    return 0;
}
