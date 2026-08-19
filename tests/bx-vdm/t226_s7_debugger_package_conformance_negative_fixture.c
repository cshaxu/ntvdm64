#include "bop-v1/bx_ntvdm_debugger_package_facade_v1.h"
#include <string.h>

int main(void)
{
    uint32_t mode;
    for (mode = 0u; mode < 16u; ++mode) {
        uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x56u, (uint8_t)mode };
        bx_ntvdm_instruction_window_v1 window; bx_ntvdm_bop_ingress_v1 ingress;
        bx_ntvdm_bop_provider_selection_v1 selection; bx_ntvdm_exception_event_v1 event;
        bx_ntvdm_cpu_state_v1 cpu; bx_ntvdm_cpu_result_v2 result;
        memset(&event, 0, sizeof(event)); event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
        event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION; event.struct_bytes = sizeof(event);
        event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event.vector = 6u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
        if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) || ingress.has_service != 0u ||
            !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
            !bx_ntvdm_debugger_package_facade_v1_dispatch(&ingress, &selection, &event, &cpu, &result) ||
            result.disposition != BX_NTVDM_CPU_RESULT_V2_STOP || result.resume_rip != 0u ||
            result.cpu_delta.gpr16_write_mask != 0u || result.eflags_write_mask != 0u) return (int)(mode + 1u);
    }
    return 0;
}