#include "bx_ntvdm_dem_drive_service.h"

#include <stdio.h>

int main(void)
{
    uint8_t inventory[26] = {
        0u, 0u, 3u, 5u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0xab00u;
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.cpu_id = 0u; event.vector = 6u; event.error_code = 0u;
    event.reserved0 = 0u; event.fault_rip = 0x7439u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u, 0xc4u, 0x50u, 0x0fu}, 4u);
    if (!bx_ntvdm_dem_drive_service_v1_dispatch(inventory, &event, &cpu,
            &window, &result) || !bx_ntvdm_cpu_result_v2_valid(&result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x743du || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0x0004u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 1;
    inventory[2] = 7u;
    if (bx_ntvdm_dem_drive_service_v1_dispatch(inventory, &event, &cpu,
            &window, &result)) return 2;
    puts("bx-ntvdm DEM drive service: inventory, AX and CF contract verified");
    return 0;
}
