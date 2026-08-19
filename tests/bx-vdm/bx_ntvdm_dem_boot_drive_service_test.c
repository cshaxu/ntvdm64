#include "bop-v1/bx_ntvdm_dem_boot_drive_service.h"
#include <windows.h>
#include <stdio.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x667u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_exception_result_v1 result;
    bx_ntvdm_host_drive_snapshot_v1 drives = {0};
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0xabcd0000u | 0x5500u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u, 0xc4u, 0x50u, 0x0du}, 4u);
    if (!bx_ntvdm_dem_boot_drive_service_v1_dispatch(&event, &cpu, &window,
            &result) || result.resume_rip != 0x66bu ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0x5503u) return 1;
    drives.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    drives.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    drives.present_mask = drives.admitted_mask = UINT32_C(1) << 3u;
    drives.types[3] = DRIVE_FIXED;
    if (!bx_ntvdm_dem_boot_drive_service_v2_dispatch(&drives, UINT32_MAX,
            &event, &cpu, &window, &result) || result.resume_rip != 0x66bu ||
        result.cpu_delta.gpr16_values[0] != 0x5504u) return 3;
    if (!bx_ntvdm_dem_boot_drive_service_v2_dispatch(&drives, 3u,
            &event, &cpu, &window, &result) ||
        result.cpu_delta.gpr16_values[0] != 0x5504u) return 4;
    drives.admitted_mask = 0u;
    if (bx_ntvdm_dem_boot_drive_service_v2_dispatch(&drives, UINT32_MAX,
            &event, &cpu, &window, &result)) return 5;
    window.bytes[3] = 0x0eu;
    if (bx_ntvdm_dem_boot_drive_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 2;
    puts("bx-ntvdm DEM boot-drive service: selected-host admission verified");
    return 0;
}
