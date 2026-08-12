#include "bx_ntvdm_vdd_create_user_notify_service.h"

#include <stdint.h>
#include <stdio.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0, 6, 0, 0, 0x5a70u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;
    uint8_t exact[] = { 0xc4u, 0xc4u, 0x50u, 0x36u };

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window, exact, sizeof(exact));
    if (!bx_ntvdm_vdd_create_user_notify_service_v1_dispatch(&event, &cpu,
            &window, &result) || result.resume_rip != 0x5a74u ||
        result.cpu_delta.gpr16_write_mask != 0u || result.eflags_write_mask != 0u)
        return 1;
    cpu.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    if (bx_ntvdm_vdd_create_user_notify_service_v1_dispatch(&event, &cpu,
            &window, &result)) return 2;
    cpu.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    exact[3] = 0x37u;
    bx_ntvdm_instruction_window_v1_capture(&window, exact, sizeof(exact));
    if (bx_ntvdm_vdd_create_user_notify_service_v1_dispatch(&event, &cpu,
            &window, &result)) return 3;
    puts("bx-ntvdm VDD create-user notification: empty VDD-list return verified");
    return 0;
}
