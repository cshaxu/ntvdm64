#include "bop-v1/bx_ntvdm_cpu_result_bridge_v1.h"
#include "bop-v1/bx_ntvdm_dem_dta_service.h"
#include <stdio.h>

int bx_ntvdm_startup_transaction_bridge_v1_copy_host_drive_inventory(
    uint8_t out_types[26])
{
    (void)out_types;
    return 0;
}

int bx_ntvdm_startup_transaction_bridge_v1_store_dta_registration(
    const bx_ntvdm_dem_dta_registration_v1 *registration)
{
    (void)registration;
    return 0;
}

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x516u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u, 0xc4u, 0x12u}, 3u);
    if (!bx_ntvdm_cpu_result_bridge_v1_dispatch(&event, &cpu, &window,
            &result) || !bx_ntvdm_cpu_result_v2_valid(&result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x519u || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0x027fu ||
        result.eflags_write_mask != 0u) return 1;
    window.bytes[2] = 0x13u;
    if (!bx_ntvdm_cpu_result_bridge_v1_dispatch(&event, &cpu, &window,
            &result) || result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH ||
        result.cpu_delta.gpr16_write_mask != 0u) return 2;
    cpu.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    window.bytes[2] = 0x12u;
    if (!bx_ntvdm_cpu_result_bridge_v1_dispatch(&event, &cpu, &window,
            &result) || result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH) return 3;
    puts("bx-ntvdm CPU-result bridge: exact BOP 12h result verified");
    return 0;
}
