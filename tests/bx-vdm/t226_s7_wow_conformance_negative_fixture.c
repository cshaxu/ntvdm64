#include "bop-v1/bx_ntvdm_top_level_package_facade_v1.h"
#include <string.h>
int main(void)
{
    uint8_t bytes[3] = { 0xc4u, 0xc4u, 0x51u };
    bx_ntvdm_instruction_window_v1 window; bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection; bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu; bx_ntvdm_cpu_result_v2 result; uint32_t route;
    memset(&event, 0, sizeof(event)); event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION; event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event.vector = 6u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    return bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) &&
        ingress.family == BX_NTVDM_BOP_FAMILY_WOW && ingress.has_service == 0u &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_top_level_package_facade_v1_classify(&ingress, &selection, &route) &&
        route == BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL &&
        bx_ntvdm_top_level_package_facade_v1_dispatch(route, &event, &cpu, &result) &&
        result.disposition == BX_NTVDM_CPU_RESULT_V2_STOP && result.resume_rip == 0u ? 0 : 1;
}