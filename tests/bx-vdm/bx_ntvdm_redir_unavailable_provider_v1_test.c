#include "bx_ntvdm_redir_unavailable_provider_v1.h"

int main(void)
{
    uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x02u, 0x00u };
    bx_ntvdm_exception_event_v1 event = {
        BX_NTVDM_EXCEPTION_ABI_MAGIC, BX_NTVDM_EXCEPTION_ABI_VERSION,
        sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x740u
    };
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_cpu_result_v2 result;
    int failed = 0;

    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress);
    failed |= ingress.route != BX_NTVDM_BOP_ROUTE_UNKNOWN_SELECTOR ||
        ingress.family != BX_NTVDM_BOP_FAMILY_NONE || ingress.selector != 2u ||
        ingress.has_service != 0u;
    failed |= !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection);
    failed |= bx_ntvdm_redir_unavailable_provider_v1_dispatch(&ingress,
        &selection, &event, &state, &result);
    bytes[2] = 0x52u;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress);
    failed |= ingress.route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
        ingress.family != BX_NTVDM_BOP_FAMILY_XMS || ingress.selector != 0x52u ||
        ingress.service != 0u || ingress.has_service != 1u;
    failed |= !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection);
    failed |= bx_ntvdm_redir_unavailable_provider_v1_dispatch(&ingress,
        &selection, &event, &state, &result);
    bytes[2] = 0x57u;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress);
    failed |= ingress.route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
        ingress.family != BX_NTVDM_BOP_FAMILY_REDIR || ingress.selector != 0x57u ||
        ingress.service != 0u || ingress.has_service != 1u;
    failed |= !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection);
    failed |= bx_ntvdm_redir_unavailable_provider_v1_dispatch(&ingress,
        &selection, &event, &state, &result);
    failed |= result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x743u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 1u;
    bytes[2] = 0x01u;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress);
    failed |= !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection);
    failed |= bx_ntvdm_redir_unavailable_provider_v1_dispatch(&ingress,
        &selection, &event, &state, &result);
    return failed ? 1 : 0;
}
