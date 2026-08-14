#include "bx_ntvdm_redir_package_facade_v1.h"

#include <string.h>

static void event_initialize(bx_ntvdm_exception_event_v1 *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->vector = 6u;
    event->fault_rip = 0x100u;
}

int main(void)
{
    uint32_t service;
    int failed = 0;
    for (service = 0u; service < 50u; ++service) {
        uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x57u, (uint8_t)service };
        bx_ntvdm_instruction_window_v1 window;
        bx_ntvdm_bop_ingress_v1 ingress;
        bx_ntvdm_bop_provider_selection_v1 selection;
        bx_ntvdm_exception_event_v1 event;
        bx_ntvdm_cpu_state_v1 cpu;
        bx_ntvdm_cpu_result_v2 result;
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
        event_initialize(&event);
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
            !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
            !bx_ntvdm_redir_package_facade_v1_dispatch(&ingress, &selection,
                &event, &cpu, &result) ||
            result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
            result.resume_rip != 0x104u ||
            result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
            result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
            result.cpu_delta.gpr16_write_mask != 1u ||
            result.cpu_delta.gpr16_values[0] != 1u;
    }
    {
        uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x57u, 0x32u };
        bx_ntvdm_instruction_window_v1 window;
        bx_ntvdm_bop_ingress_v1 ingress;
        bx_ntvdm_bop_provider_selection_v1 selection;
        bx_ntvdm_exception_event_v1 event;
        bx_ntvdm_cpu_state_v1 cpu;
        bx_ntvdm_cpu_result_v2 result;
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
        event_initialize(&event);
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
            !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
            bx_ntvdm_redir_package_facade_v1_dispatch(&ingress, &selection,
                &event, &cpu, &result);
    }
    return failed ? 1 : 0;
}
