#include "controlled_stop_service.h"

#include <stdio.h>

int main(void)
{
    runtime_exception_event_v1 event = { RUNTIME_EXCEPTION_ABI_MAGIC,
        RUNTIME_EXCEPTION_ABI_VERSION, sizeof(event),
        RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x100u };
    runtime_cpu_state_v1 cpu;
    runtime_instruction_window_v1 window;
    runtime_cpu_result_v2 result;

    runtime_cpu_state_v1_initialize(&cpu, RUNTIME_CPU_EXECUTION_REAL);
    runtime_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, 0xfeu, 0xe3u }, 4u);
    if (!runtime_controlled_stop_service_v1_dispatch(&event, &cpu, &window,
            &result) || result.disposition != RUNTIME_CPU_RESULT_V2_STOP ||
        result.resume_rip != 0u || result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != 0u || !runtime_cpu_result_v2_valid(&result))
        return 1;
    window.bytes[2] = 0xfdu;
    if (runtime_controlled_stop_service_v1_dispatch(&event, &cpu, &window, &result))
        return 2;
    window.bytes[2] = 0xfeu;
    cpu.execution_mode = RUNTIME_CPU_EXECUTION_PROTECTED;
    if (runtime_controlled_stop_service_v1_dispatch(&event, &cpu, &window, &result))
        return 3;
    puts("bx-ntvdm controlled-stop: exact real-mode terminal contract verified");
    return 0;
}
