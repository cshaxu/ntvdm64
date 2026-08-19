#include "bop-v1/bx_ntvdm_dem_debug_service.h"
#include <stdio.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x55cu };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_exception_result_v1 result;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0x0000a5ffu;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u, 0xc4u, 0x50u, 0x3bu}, 4u);
    if (!bx_ntvdm_dem_debug_service_v1_dispatch(&event, &cpu, &window, &result) ||
        result.resume_rip != 0x560u || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0xa500u) return 1;
    window.bytes[3] = 0x3cu;
    if (bx_ntvdm_dem_debug_service_v1_dispatch(&event, &cpu, &window, &result)) return 2;
    puts("bx-ntvdm DEM debug service: non-invasive CLI false result verified");
    return 0;
}
