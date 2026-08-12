#include "bx_ntvdm_dem_boot_drive_service.h"
#include <stdio.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x667u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_exception_result_v1 result;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0xabcd0000u | 0x5500u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u, 0xc4u, 0x50u, 0x0du}, 4u);
    if (!bx_ntvdm_dem_boot_drive_service_v1_dispatch(&event, &cpu, &window,
            &result) || result.resume_rip != 0x66bu ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0x5503u) return 1;
    window.bytes[3] = 0x0eu;
    if (bx_ntvdm_dem_boot_drive_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 2;
    puts("bx-ntvdm DEM boot-drive service: noninvasive C fallback verified");
    return 0;
}
