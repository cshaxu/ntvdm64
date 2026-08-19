#include "bop-v1/bx_ntvdm_dem_provider_v1.h"

int main(void)
{
    static const uint8_t original_noops[] = { 31u, 36u, 38u, 40u, 43u, 64u, 66u, 67u };
    bx_ntvdm_exception_event_v1 event = {
        BX_NTVDM_EXCEPTION_ABI_MAGIC, BX_NTVDM_EXCEPTION_ABI_VERSION,
        sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x400u
    };
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_plane_record_v1 plane;
    bx_ntvdm_cpu_result_v2 result;
    uint32_t index;
    int failed = 0;

    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    for (index = 0u; index < sizeof(original_noops); ++index) {
        uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, original_noops[index] };
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4u);
        failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress);
        failed |= !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection);
        failed |= !bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &plane);
        failed |= !bx_ntvdm_dem_provider_v1_dispatch(&ingress, &selection, &plane,
            &event, &state, &result);
        failed |= result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
            result.resume_rip != 0x404u ||
            result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
            result.eflags_values != 0u;
    }
    {
        uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, 22u };
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4u);
    }
    failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress);
    failed |= !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection);
    failed |= !bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &plane);
    failed |= bx_ntvdm_dem_provider_v1_dispatch(&ingress, &selection, &plane,
        &event, &state, &result);
    {
        uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, 31u };
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4u);
        failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress);
        failed |= !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection);
        failed |= !bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &plane);
        plane.component = BX_NTVDM_DEM_COMPONENT_GSET;
        failed |= bx_ntvdm_dem_provider_v1_dispatch(&ingress, &selection, &plane,
            &event, &state, &result);
    }
    return failed ? 1 : 0;
}
