#include "bx_ntvdm_dem_ioctl_metadata_provider_v1.h"

#include <stdio.h>

static int dispatch(uint8_t selector, uint8_t service, uint16_t ax,
    uint8_t drive, uint8_t type, int admitted,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint8_t bytes[4] = {0xc4u, 0xc4u, selector, service};
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_plane_record_v1 plane;
    bx_ntvdm_host_drive_snapshot_v1 snapshot;
    bx_ntvdm_exception_event_v1 event = {BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x443cu};
    bx_ntvdm_cpu_state_v1 cpu;
    uint8_t types[26] = {0};

    types[drive] = type;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(UINT32_C(1) << drive, types,
            0u, 0u, &snapshot) || !bx_ntvdm_bop_ingress_v1_classify(
            &window, &ingress) || !bx_ntvdm_bop_provider_registry_v1_select(
            &ingress, &selection) || !bx_ntvdm_dem_plane_v1_classify(
            &ingress, &selection, &plane)) return 0;
    if (!admitted) {
        snapshot.admitted_mask = 0u;
        snapshot.types[drive] = 0u;
    }
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = ax;
    cpu.ebx = drive;
    return bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch(&ingress,
        &selection, &plane, &snapshot, &event, &cpu, result);
}

int main(void)
{
    bx_ntvdm_cpu_result_v2 result;
    if (!dispatch(0x50u, 0x21u, 0x4408u, 2u, 3u, 1, &result) || result.resume_rip != 0x4440u ||
        result.cpu_delta.gpr16_values[0] != 1u || result.eflags_values != 0u)
        return 1;
    if (!dispatch(0x50u, 0x21u, 0x4408u, 2u, 2u, 1, &result) ||
        result.cpu_delta.gpr16_values[0] != 0u) return 2;
    if (!dispatch(0x50u, 0x21u, 0x4409u, 2u, 4u, 1, &result) ||
        result.cpu_delta.gpr16_values[0] != 0x4400u ||
        result.cpu_delta.gpr16_values[3] != 0x1000u) return 3;
    if (!dispatch(0x50u, 0x21u, 0x4409u, 2u, 5u, 1, &result) ||
        result.cpu_delta.gpr16_values[3] != 0x1000u) return 4;
    if (!dispatch(0x50u, 0x21u, 0x4409u, 2u, 3u, 1, &result) ||
        result.cpu_delta.gpr16_values[3] != 0x0800u) return 5;
    if (dispatch(0x50u, 0x21u, 0x440du, 2u, 3u, 1, &result) ||
        dispatch(0x50u, 0x21u, 0x4411u, 2u, 3u, 1, &result) ||
        dispatch(0x50u, 0x21u, 0x4408u, 2u, 3u, 0, &result) ||
        dispatch(0x50u, 0x46u, 0x4408u, 2u, 3u, 1, &result) ||
        dispatch(0x54u, 0x05u, 0x4408u, 2u, 3u, 1, &result)) return 6;
    puts("bx-ntvdm DEM IOCTL metadata provider: paired snapshot contracts verified");
    return 0;
}
