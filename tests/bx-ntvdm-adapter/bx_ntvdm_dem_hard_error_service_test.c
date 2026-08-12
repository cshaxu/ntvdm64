#include <stdio.h>

#include "bx_ntvdm_dem_hard_error_service.h"

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x7490u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_dem_hard_error_registration_v1 registration;
    bx_ntvdm_cpu_result_v2 result;
    const uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, 0x32u };
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x00a7u; cpu.edx = 0x0332u; cpu.ebx = 0x0070u;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    if (!bx_ntvdm_dem_hard_error_service_v1_dispatch(&event, &cpu, &window,
            &registration, &result) || result.resume_rip != 0x7494u ||
        registration.hard_error_packet != 0x0da2u ||
        registration.device_chain != 0x0ae0u) return 1;
    window.bytes[3] = 0x33u;
    if (bx_ntvdm_dem_hard_error_service_v1_dispatch(&event, &cpu, &window,
            &registration, &result)) return 2;
    puts("bx-ntvdm DemSetHardErrorInfo: source-derived locator registration verified");
    return 0;
}
