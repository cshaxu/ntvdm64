#include "bx_ntvdm_dem_ioctl_metadata_provider_v1.h"

#define BX_NTVDM_DEM_IOCTL_SERVICE 0x21u
#define BX_NTVDM_DEM_IOCTL_CHANGEABLE 0x4408u
#define BX_NTVDM_DEM_IOCTL_LOCATION_OR_REMOTE 0x4409u
#define BX_NTVDM_DEM_ERROR_INVALID_DRIVE 15u
#define BX_NTVDM_GPR16_AX 0u
#define BX_NTVDM_GPR16_DX 3u

static int bx_ntvdm_dem_ioctl_metadata_provider_v1_matches(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before)
{
    return bx_ntvdm_bop_ingress_v1_valid(ingress) &&
        bx_ntvdm_bop_provider_selection_v1_valid(selection) &&
        bx_ntvdm_dem_plane_v1_valid(plane) &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu_before) &&
        selection->disposition == BX_NTVDM_BOP_PROVIDER_DEFERRED &&
        selection->provider_family == BX_NTVDM_BOP_PROVIDER_DEM &&
        selection->precedence == BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT &&
        ingress->route == BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED &&
        ingress->family == BX_NTVDM_BOP_FAMILY_DEM &&
        ingress->service == BX_NTVDM_DEM_IOCTL_SERVICE &&
        plane->service == ingress->service &&
        plane->component == BX_NTVDM_DEM_COMPONENT_RAW_MEDIA &&
        plane->disposition == BX_NTVDM_DEM_PLANE_DEFERRED &&
        event->kind == BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION &&
        event->vector == 6u &&
        cpu_before->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u;
}

int bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint16_t function;
    uint8_t drive;
    uint8_t type;

    if (result == 0 || !bx_ntvdm_dem_ioctl_metadata_provider_v1_matches(
            ingress, selection, plane, event, cpu_before) ||
        !bx_ntvdm_host_drive_snapshot_v1_valid(snapshot)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    function = (uint16_t)cpu_before->eax;
    drive = (uint8_t)cpu_before->ebx;
    if (function != BX_NTVDM_DEM_IOCTL_CHANGEABLE &&
        function != BX_NTVDM_DEM_IOCTL_LOCATION_OR_REMOTE)
        return 0;
    /* demIoctlChangeable owns both 08h and 09h, including its distinct
     * invalid-drive branch.  Do not let a recognized metadata request fall
     * through to the unrelated demIoctlInvalid AX=1 result. */
    if (drive >= 26u || (snapshot->admitted_mask & (UINT32_C(1) << drive)) == 0u ||
        (type = snapshot->types[drive]) < 2u || type > 6u)
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta,
                BX_NTVDM_GPR16_AX, BX_NTVDM_DEM_ERROR_INVALID_DRIVE) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 1);

    if (!bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u)) return 0;
    if (function == BX_NTVDM_DEM_IOCTL_CHANGEABLE) {
        if (!bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta,
                BX_NTVDM_GPR16_AX, type == 2u ? 0u : 1u)) return 0;
    } else {
        uint16_t ax = (uint16_t)cpu_before->eax & 0xff00u;
        uint16_t dx = (type == 4u || type == 5u) ? 0x1000u : 0x0800u;
        if (!bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta,
                BX_NTVDM_GPR16_AX, ax) ||
            !bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta,
                BX_NTVDM_GPR16_DX, dx)) return 0;
    }
    if (!bx_ntvdm_cpu_result_v2_set_cf(result, 0)) {
        bx_ntvdm_cpu_result_v2_pass_through(result);
        return 0;
    }
    return 1;
}
