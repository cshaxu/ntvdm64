#include "bx_ntvdm_machine_bop_facade_v1.h"

#include <string.h>

static int check(uint8_t selector, uint32_t expected, int stop)
{
    uint8_t bytes[3] = { 0xc4u, 0xc4u, selector };
    bx_ntvdm_instruction_window_v1 window; bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection; bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu; bx_ntvdm_cpu_result_v2 result; uint32_t route;
    memset(&event, 0, sizeof(event)); event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION; event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event.vector = 6u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
        !bx_ntvdm_machine_bop_facade_v1_classify(&ingress, &selection, &route) ||
        route != expected) return 0;
    if (!stop) return !bx_ntvdm_machine_bop_facade_v1_dispatch(route, &event, &cpu, &result);
    return bx_ntvdm_machine_bop_facade_v1_dispatch(route, &event, &cpu, &result) &&
        result.disposition == BX_NTVDM_CPU_RESULT_V2_STOP;
}

int main(void)
{
    return check(0x12u, BX_NTVDM_MACHINE_BOP_MEMORY, 0) &&
        check(0x15u, BX_NTVDM_MACHINE_BOP_MEMORY, 0) &&
        check(0x5fu, BX_NTVDM_MACHINE_BOP_HANDOFF, 0) &&
        check(0x5cu, BX_NTVDM_MACHINE_BOP_DEFERRED_STOP, 1) &&
        check(0x5du, BX_NTVDM_MACHINE_BOP_DEFERRED_STOP, 1) &&
        check(0xfdu, BX_NTVDM_MACHINE_BOP_DEFERRED_STOP, 1) ? 0 : 1;
}
