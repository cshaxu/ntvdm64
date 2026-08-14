#include "bx_ntvdm_dem_error_lock_plane_v1.h"

#include "bx_ntvdm_dem_plane_v1.h"

#define BX_NTVDM_DEM_ERROR_ACCESS_DENIED 5u
#define BX_NTVDM_DEM_ERROR_NO_RETRY 0xffu

void bx_ntvdm_dem_error_lock_plane_v1_clear(
    bx_ntvdm_dem_error_lock_plane_v1 *plane)
{
    if (plane == 0) return;
    plane->has_registration = 0u;
    plane->registration.hard_error_packet = 0u;
    plane->registration.device_chain = 0u;
}

int bx_ntvdm_dem_error_lock_plane_v1_dispatch(
    bx_ntvdm_dem_error_lock_plane_v1 *plane,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_dem_plane_record_v1 record;
    bx_ntvdm_dem_hard_error_registration_v1 registration;
    if (plane == 0 || ingress == 0 || selection == 0 || event == 0 ||
        cpu_before == 0 || window == 0 || result == 0 ||
        !bx_ntvdm_dem_plane_v1_classify(ingress, selection, &record) ||
        record.component != BX_NTVDM_DEM_COMPONENT_ERROR_LOCK ||
        record.disposition != BX_NTVDM_DEM_PLANE_DEFERRED) return 0;
    if (record.service == 0x32u) {
        if (!bx_ntvdm_dem_hard_error_service_v1_dispatch(event, cpu_before,
                window, &registration, result)) return 0;
        plane->registration = registration;
        plane->has_registration = 1u;
        return 1;
    }
    if (event->vector != 6u || cpu_before->execution_mode !=
        BX_NTVDM_CPU_EXECUTION_REAL || event->fault_rip > UINT64_MAX - 4u)
        return 0;
    if (record.service == 0x33u)
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u,
                BX_NTVDM_DEM_ERROR_NO_RETRY) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 1);
    if (record.service == 0x3fu)
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u,
                BX_NTVDM_DEM_ERROR_ACCESS_DENIED) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 1);
    return 0;
}
