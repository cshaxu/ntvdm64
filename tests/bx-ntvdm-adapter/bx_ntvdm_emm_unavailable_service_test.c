#include "bx_ntvdm_emm_unavailable_service.h"

#include <stdint.h>
#include <stdio.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x79c5u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ebx = 0xffffu;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u, 0xc4u, 0x66u}, 3u);
    if (!bx_ntvdm_emm_unavailable_service_v1_dispatch(&event, &cpu, &window,
            &result) || result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x79c8u || result.cpu_delta.gpr16_write_mask !=
            (1u << 3u) || result.cpu_delta.gpr16_values[3] != 0u ||
        result.eflags_write_mask != 0u || result.eflags_values != 0u) return 1;
    window.bytes[2] = 0x67u;
    if (bx_ntvdm_emm_unavailable_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 2;
    window.bytes[2] = 0x66u;
    cpu.execution_mode = BX_NTVDM_CPU_EXECUTION_V8086;
    if (bx_ntvdm_emm_unavailable_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 3;
    cpu.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event.fault_rip = UINT64_MAX - 2u;
    if (bx_ntvdm_emm_unavailable_service_v1_dispatch(&event, &cpu, &window,
            &result)) return 4;
    puts("bx-ntvdm EMM unavailable service: exact BOP 66 unavailable result verified");
    return 0;
}
