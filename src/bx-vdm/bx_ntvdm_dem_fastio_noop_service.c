#include "bx_ntvdm_dem_fastio_noop_service.h"

#include <stdint.h>

int bx_ntvdm_dem_fastio_noop_service_v1_dispatch(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint8_t service;
    if (event == 0 || cpu == 0 || window == 0 || result == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) || !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        !bx_ntvdm_instruction_window_v1_valid(window) ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION || event->vector != 6u ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL || event->fault_rip > UINT64_MAX - 4u ||
        window->valid_bytes < 4u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != 0x50u) return 0;
    service = window->bytes[3];
    if (service != 0x42u && service != 0x43u) return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 0);
}
