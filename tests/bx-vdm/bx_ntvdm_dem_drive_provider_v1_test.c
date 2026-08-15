#include "bx_ntvdm_dem_drive_provider_v1.h"

static int test_snapshot(uint32_t present_mask, const uint8_t types[26],
    uint16_t expected)
{
    bx_ntvdm_host_drive_snapshot_v1 snapshot;
    bx_ntvdm_exception_event_v1 event = {
        BX_NTVDM_EXCEPTION_ABI_MAGIC, BX_NTVDM_EXCEPTION_ABI_VERSION,
        sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x800u
    };
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_plane_record_v1 plane;
    bx_ntvdm_cpu_result_v2 result;
    uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, 15u };
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(present_mask, types, 0u, 0u,
            &snapshot)) return 0;
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4u);
    return bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &plane) &&
        bx_ntvdm_dem_drive_provider_v1_dispatch(&ingress, &selection, &plane, &snapshot,
            &event, &state, &result) &&
        result.disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result.resume_rip == 0x804u &&
        result.cpu_delta.gpr16_write_mask == 1u &&
        result.cpu_delta.gpr16_values[0] == expected &&
        result.eflags_write_mask == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF &&
        result.eflags_values == 0u;
}

int main(void)
{
    uint8_t contiguous[26] = { 0 };
    uint8_t gap[26] = { 0 };
    contiguous[2] = 3u; contiguous[3] = 2u; contiguous[4] = 5u;
    gap[2] = 3u; gap[4] = 3u;
    return !test_snapshot((UINT32_C(1) << 2) | (UINT32_C(1) << 3) |
        (UINT32_C(1) << 4), contiguous, 5u) ||
        !test_snapshot((UINT32_C(1) << 2) | (UINT32_C(1) << 4), gap, 5u);
}
