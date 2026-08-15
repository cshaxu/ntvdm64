#include "bx_ntvdm_command_launch_execution_provider_v1.h"
#include "bx_ntvdm_command_plane_v1.h"

static void event_initialize(bx_ntvdm_exception_event_v1 *event)
{
    *event = (bx_ntvdm_exception_event_v1){ BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(*event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x200u };
}

int main(void)
{
    bx_ntvdm_command_launch_execution_provider_v1 provider;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;
    uint32_t service;
    const uint8_t expected[17] = {
        0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,0,0
    };
    if (!bx_ntvdm_command_launch_execution_provider_v1_initialize(&provider) ||
        !bx_ntvdm_command_launch_execution_provider_v1_valid(&provider)) return 1;
    for (service = 0u; service != 17u; ++service)
        if (bx_ntvdm_command_launch_execution_provider_v1_owns_service((uint8_t)service) != expected[service]) return 2;
    if (bx_ntvdm_command_launch_execution_provider_v1_owns_service(17u)) return 3;

    event_initialize(&event);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    { const uint8_t bytes[4] = {0xc4u,0xc4u,0x54u,6u};
      bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes)); }
    cpu.ecx = 0u;
    if (!bx_ntvdm_command_stream_child_v1_dispatch_stream(&provider.stream_child,
            0, &event, &cpu, &window, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
        result.cpu_delta.gpr16_values[1] != 0u || result.cpu_delta.gpr16_values[3] != 0u) return 4;

    { const uint8_t bytes[4] = {0xc4u,0xc4u,0x54u,7u};
      bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes)); }
    if (!bx_ntvdm_command_execution_lifecycle_v1_dispatch(&provider.execution,
            0, 0, &event, &cpu, &window, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.eflags_values != 0u) return 5;

    { const uint8_t bytes[4] = {0xc4u,0xc4u,0x54u,8u};
      bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes)); }
    cpu.eax = 0xa500u;
    if (!bx_ntvdm_command_execution_lifecycle_v1_dispatch(&provider.execution,
            0, 0, &event, &cpu, &window, &result) ||
        result.cpu_delta.gpr16_values[0] != 0xa532u || result.eflags_values != 0u) return 6;

    { const uint8_t bytes[4] = {0xc4u,0xc4u,0x54u,10u};
      bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes)); }
    if (!bx_ntvdm_command_execution_lifecycle_v1_dispatch(&provider.execution,
            0, 0, &event, &cpu, &window, &result) ||
        result.cpu_delta.gpr16_values[0] != 0xa532u || result.eflags_values != 0u ||
        provider.execution.deferred_execution_count != 2u) return 7;
    return 0;
}
