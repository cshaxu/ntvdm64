#include "bx_ntvdm_controlled_stop_service.h"

int bx_ntvdm_controlled_stop_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (!event || !cpu || !window || !result ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL || window->valid_bytes < 3u ||
        window->bytes[0] != 0xc4u || window->bytes[1] != 0xc4u ||
        window->bytes[2] != 0xfeu) return 0;
    return bx_ntvdm_cpu_result_v2_stop(result);
}
