#include "bop-v1/bx_ntvdm_spckbd_init_service.h"

#include <stdio.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x4567u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0x1234beefu;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u, 0xc4u, 0x5fu}, 3u);
    if (!bx_ntvdm_spckbd_init_service_v1_dispatch(&event, &cpu, &window,
            &result) || result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x456au || result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 1;
    cpu.eax = 0xbeeeu;
    if (bx_ntvdm_spckbd_init_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 2;
    cpu.eax = 0xbeefu;
    cpu.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    if (bx_ntvdm_spckbd_init_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 3;
    cpu.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    window.bytes[2] = 0x5au;
    if (bx_ntvdm_spckbd_init_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 4;
    window.bytes[2] = 0x5fu;
    event.fault_rip = UINT64_MAX - 2u;
    if (bx_ntvdm_spckbd_init_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 5;
    puts("bx-ntvdm SPCKBD init service: exact BOP 5F x86 continuation verified");
    return 0;
}
