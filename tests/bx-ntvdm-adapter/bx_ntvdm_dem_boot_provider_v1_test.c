#include "bx_ntvdm_dem_boot_provider_v1.h"

int main(void)
{
    bx_ntvdm_exception_event_v1 event = {
        BX_NTVDM_EXCEPTION_ABI_MAGIC, BX_NTVDM_EXCEPTION_ABI_VERSION,
        sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x666u
    };
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_cpu_result_v2 result;
    uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, 13u };

    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    state.eax = 0xabcd5500u;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4u);
    if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
        !bx_ntvdm_dem_boot_provider_v1_dispatch(&ingress, &selection, 2u,
            &event, &state, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x66au ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0x5503u) return 1;
    if (bx_ntvdm_dem_boot_provider_v1_dispatch(&ingress, &selection, 26u,
            &event, &state, &result)) return 2;
    return 0;
}
