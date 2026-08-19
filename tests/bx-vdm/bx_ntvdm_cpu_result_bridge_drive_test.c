#include "bop-v1/bx_ntvdm_cpu_result_bridge_v1.h"
#include "bop-v1/bx_ntvdm_dem_dta_service.h"
#include <stdio.h>

int bx_ntvdm_startup_transaction_bridge_v1_copy_host_drive_inventory(
    uint8_t out_types[26])
{
    uint32_t index;
    if (out_types == 0) return 0;
    for (index = 0u; index < 26u; ++index) out_types[index] = 0u;
    return 1;
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
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x7439u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u, 0xc4u, 0x50u, 0x0fu}, 4u);
    if (!bx_ntvdm_cpu_result_bridge_v1_dispatch(&event, &cpu, &window,
            &result) || result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x743du || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 2u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 1;
    puts("bx-ntvdm CPU-result bridge: frozen all-zero drive inventory verified");
    return 0;
}
