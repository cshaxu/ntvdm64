#include "bop-v1/bx_ntvdm_top_level_package_facade_v1.h"

#include <string.h>

static int check(uint8_t selector, uint32_t expected_route,
    uint32_t expected_disposition, int expect_facade_result)
{
    uint8_t bytes[3] = { 0xc4u, 0xc4u, selector };
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    uint32_t route;
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.vector = 6u; event.fault_rip = 0x100u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
        !bx_ntvdm_top_level_package_facade_v1_classify(&ingress, &selection,
            &route) || route != expected_route) return 0;
    if (!expect_facade_result)
        return !bx_ntvdm_top_level_package_facade_v1_dispatch(route, &event,
            &cpu, &result);
    if (!bx_ntvdm_top_level_package_facade_v1_dispatch(route, &event, &cpu,
            &result) || result.disposition != expected_disposition) return 0;
    return expected_disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        (result.resume_rip == 0x103u && result.cpu_delta.gpr16_write_mask == 0u &&
         result.eflags_write_mask == 0u);
}

int main(void)
{
    if (!check(0x51u, BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL,
            BX_NTVDM_CPU_RESULT_V2_STOP, 1)) return 1;
    if (!check(0x59u, BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL,
            BX_NTVDM_CPU_RESULT_V2_STOP, 1)) return 2;
    if (!check(0x5au, BX_NTVDM_TOP_LEVEL_PACKAGE_IDLE,
            BX_NTVDM_CPU_RESULT_V2_RESUME, 1)) return 3;
    if (!check(0x5bu, BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL,
            BX_NTVDM_CPU_RESULT_V2_STOP, 1)) return 4;
    if (!check(0x5eu, BX_NTVDM_TOP_LEVEL_PACKAGE_CONFIG,
            BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH, 0)) return 5;
    {
        uint8_t bytes[3] = { 0xc4u, 0xc4u, 0x5eu };
        bx_ntvdm_instruction_window_v1 window;
        bx_ntvdm_bop_ingress_v1 ingress;
        bx_ntvdm_bop_provider_selection_v1 selection;
        bx_ntvdm_exception_event_v1 event;
        bx_ntvdm_cpu_state_v1 cpu;
        bx_ntvdm_cpu_result_v2 result;
        uint32_t route;
        memset(&event, 0, sizeof(event));
        event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
        event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
        event.struct_bytes = sizeof(event);
        event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
        event.vector = 6u; event.fault_rip = 0x100u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.eax = 1u;
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
        if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
            !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
            !bx_ntvdm_top_level_package_facade_v1_classify(&ingress, &selection,
                &route) || !bx_ntvdm_top_level_package_facade_v1_dispatch(route,
                &event, &cpu, &result) || result.disposition !=
                BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x103u ||
            result.cpu_delta.gpr16_write_mask != 0u ||
            result.eflags_write_mask != 0u) return 7;
    }
    if (!check(0xfeu, BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL,
            BX_NTVDM_CPU_RESULT_V2_STOP, 1)) return 6;
    return 0;
}
