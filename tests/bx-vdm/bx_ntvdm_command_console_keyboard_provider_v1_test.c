#include "bop-v1/bx_ntvdm_command_console_keyboard_provider_v1.h"

static void event_initialize(bx_ntvdm_exception_event_v1 *event)
{
    *event=(bx_ntvdm_exception_event_v1){BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION,sizeof(*event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,0u,6u,0u,0u,0x5d9u};
}

static void window_initialize(bx_ntvdm_instruction_window_v1 *window,uint8_t service)
{
    const uint8_t bytes[4]={0xc4u,0xc4u,0x54u,service};
    bx_ntvdm_instruction_window_v1_capture(window,bytes,sizeof(bytes));
}

int main(void)
{
    bx_ntvdm_command_console_keyboard_provider_v1 provider;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;
    if (!bx_ntvdm_command_console_keyboard_provider_v1_initialize(&provider) ||
        !bx_ntvdm_command_console_keyboard_provider_v1_valid(&provider) ||
        !bx_ntvdm_command_console_keyboard_provider_v1_owns_service(9u) ||
        !bx_ntvdm_command_console_keyboard_provider_v1_owns_service(14u) ||
        bx_ntvdm_command_console_keyboard_provider_v1_owns_service(16u)) return 1;
    event_initialize(&event);
    bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL);
    window_initialize(&window,9u);
    if (!bx_ntvdm_command_console_keyboard_provider_v1_dispatch(&provider,
            &event,&cpu,&window,&result) || result.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip!=event.fault_rip+4u || provider.console.initialized!=1u ||
        result.cpu_delta.gpr16_write_mask!=0u) return 2;
    provider.console.initialized=0u;
    cpu.edx=0xffffu;
    window_initialize(&window,14u);
    if (!bx_ntvdm_command_console_keyboard_provider_v1_dispatch(&provider,
            &event,&cpu,&window,&result) || result.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip!=event.fault_rip+4u || provider.console.initialized!=1u ||
        result.cpu_delta.gpr16_write_mask!=(1u<<2u) || result.cpu_delta.gpr16_values[2]!=0u) return 3;
    window_initialize(&window,16u);
    if (bx_ntvdm_command_console_keyboard_provider_v1_dispatch(&provider,
            &event,&cpu,&window,&result)) return 4;
    cpu.execution_mode=BX_NTVDM_CPU_EXECUTION_PROTECTED;
    window_initialize(&window,14u);
    if (bx_ntvdm_command_console_keyboard_provider_v1_dispatch(&provider,
            &event,&cpu,&window,&result)) return 5;
    return 0;
}
