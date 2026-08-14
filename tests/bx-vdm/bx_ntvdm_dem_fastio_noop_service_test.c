#include "bx_ntvdm_dem_fastio_noop_service.h"

int main(void)
{
    static const uint8_t services[] = { 0x42u, 0x43u };
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,
        0u, 6u, 0u, 0u, 0x7400u };
    bx_ntvdm_cpu_state_v1 cpu; bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result; unsigned int index;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0xaaaa4e53u; cpu.ebp = 0xbbbb4005u; cpu.eflags = 0x46u;
    for (index = 0u; index < sizeof(services); ++index) {
        const uint8_t bytes[] = { 0xc4u, 0xc4u, 0x50u, services[index] };
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
        if (!bx_ntvdm_dem_fastio_noop_service_v1_dispatch(&event, &cpu, &window, &result) ||
            result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x7404u ||
            result.cpu_delta.gpr16_write_mask != 0u ||
            result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
            result.eflags_values != 0u) return 1;
    }
    window.bytes[3] = 0x41u;
    if (bx_ntvdm_dem_fastio_noop_service_v1_dispatch(&event, &cpu, &window, &result)) return 2;
    event.vector = 13u; window.bytes[3] = 0x42u;
    return bx_ntvdm_dem_fastio_noop_service_v1_dispatch(&event, &cpu, &window, &result) ? 3 : 0;
}
