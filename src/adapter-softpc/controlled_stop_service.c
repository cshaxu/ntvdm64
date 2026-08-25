#include "controlled_stop_service.h"

int runtime_controlled_stop_service_dispatch(
    const runtime_exception_event *event,
    const runtime_cpu_state *cpu,
    const runtime_instruction_window *window,
    runtime_cpu_result *result)
{
    if (!event || !cpu || !window || !result ||
        !runtime_exception_event_valid(event) ||
        !runtime_cpu_state_valid(cpu) ||
        !runtime_instruction_window_valid(window) || event->vector != 6u ||
        cpu->execution_mode != RUNTIME_CPU_EXECUTION_REAL || window->valid_bytes < 3u ||
        window->bytes[0] != 0xc4u || window->bytes[1] != 0xc4u ||
        window->bytes[2] != 0xfeu) return 0;
    return runtime_cpu_result_stop(result);
}
