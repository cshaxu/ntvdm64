#include "bop-v1/bx_ntvdm_wait_if_idle_service.h"

#include <stdio.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x5a0u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_exception_result_v1 result;

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0xbeef0000u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u, 0xc4u, 0x5au}, 3u);
    if (!bx_ntvdm_wait_if_idle_service_v1_dispatch(&event, &cpu, &window,
            &result) || result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
        result.resume_rip != 0x5a3u ||
        result.cpu_delta.gpr16_write_mask != 0u ||
        result.cpu_delta.reserved0 != 0u) return 1;
    cpu.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    if (bx_ntvdm_wait_if_idle_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 2;
    cpu.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    window.bytes[2] = 0x5fu;
    if (bx_ntvdm_wait_if_idle_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 3;
    window.bytes[2] = 0x5au;
    event.fault_rip = UINT64_MAX - 2u;
    if (bx_ntvdm_wait_if_idle_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 4;
    puts("bx-ntvdm wait-if-idle service: exact no-state BOP 5A resume verified");
    return 0;
}
