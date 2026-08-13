#include "bx_ntvdm_spckbd_init_service.h"

#include <stdint.h>

#define BX_NTVDM_SPCKBD_INIT_VERSION 0xbeefu

int bx_ntvdm_spckbd_init_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (event == 0 || cpu_before == 0 || window == 0 || result == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        (cpu_before->eax & 0xffffu) != BX_NTVDM_SPCKBD_INIT_VERSION ||
        window->valid_bytes < 3u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != 0x5fu ||
        event->fault_rip > UINT64_MAX - 3u)
        return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 3u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1);
}
