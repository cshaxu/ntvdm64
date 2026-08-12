#include "bx_ntvdm_bios_memory_service.h"
#include <stdio.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x516u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_exception_result_v1 result;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u,0xc4u,0x12u}, 3u);
    if (!bx_ntvdm_bios_memory_service_v1_dispatch(&event, &cpu, &window, &result) ||
        result.resume_rip != 0x519u || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0x027fu) return 1;
    event.fault_rip = 0x655u;
    cpu.eax = 0x8800u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u,0xc4u,0x15u}, 3u);
    if (!bx_ntvdm_bios_memory_service_v1_dispatch(&event, &cpu, &window, &result) ||
        result.resume_rip != 0x658u || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0x0c00u) return 2;
    cpu.eax = 0x8600u;
    if (bx_ntvdm_bios_memory_service_v1_dispatch(&event, &cpu, &window, &result)) return 3;
    window.bytes[2] = 0x13u;
    if (bx_ntvdm_bios_memory_service_v1_dispatch(&event, &cpu, &window, &result)) return 4;
    puts("bx-ntvdm BIOS memory service: exact BOP 12h and 15h/AH=88h contracts verified");
    return 0;
}
