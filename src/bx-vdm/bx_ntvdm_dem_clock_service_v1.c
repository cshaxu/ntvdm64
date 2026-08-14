#include "bx_ntvdm_dem_clock_service_v1.h"

#include <time.h>

static int selected(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return event && cpu && window && bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) && bx_ntvdm_instruction_window_v1_valid(window) &&
        event->vector == 6u && cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        window->valid_bytes >= 4u && window->bytes[0] == 0xc4u &&
        window->bytes[1] == 0xc4u && window->bytes[2] == 0x50u &&
        event->fault_rip <= UINT64_MAX - 4u;
}

int bx_ntvdm_dem_clock_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    time_t now;
    struct tm local;
    uint16_t ax, cx, dx;

    if (!result || !selected(event, cpu, window)) return 0;
    if (window->bytes[3] == 0x19u || window->bytes[3] == 0x1cu) {
        ax = (uint16_t)((cpu->eax & 0xff00u) | 0xffu);
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax);
    }
    if (window->bytes[3] != 0x14u && window->bytes[3] != 0x15u) return 0;
    now = time(0);
    if (now == (time_t)-1 || localtime_s(&local, &now) != 0) return 0;
    if (!bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u)) return 0;
    if (window->bytes[3] == 0x14u) {
        ax = (uint16_t)((cpu->eax & 0xff00u) | (uint8_t)local.tm_wday);
        cx = (uint16_t)(local.tm_year + 1900);
        dx = (uint16_t)(((uint16_t)(local.tm_mon + 1) << 8) | (uint8_t)local.tm_mday);
    } else {
        ax = (uint16_t)cpu->eax;
        cx = (uint16_t)(((uint16_t)local.tm_hour << 8) | (uint8_t)local.tm_min);
        dx = (uint16_t)((uint16_t)local.tm_sec << 8);
    }
    return bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, cx) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, dx);
}
