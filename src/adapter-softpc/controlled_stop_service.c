#include "controlled_stop_service.h"

int runtime_controlled_stop_service_v1_dispatch(
    const runtime_exception_event_v1 *event,
    const runtime_cpu_state_v1 *cpu,
    const runtime_instruction_window_v1 *window,
    runtime_cpu_result_v2 *result)
{
    if (!event || !cpu || !window || !result ||
        !runtime_exception_event_v1_valid(event) ||
        !runtime_cpu_state_v1_valid(cpu) ||
        !runtime_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu->execution_mode != RUNTIME_CPU_EXECUTION_REAL || window->valid_bytes < 3u ||
        window->bytes[0] != 0xc4u || window->bytes[1] != 0xc4u ||
        window->bytes[2] != 0xfeu) return 0;
    return runtime_cpu_result_v2_stop(result);
}
