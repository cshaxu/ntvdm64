#include "bx_ntvdm_dem_session_lifecycle_provider_v1.h"

#include <stdint.h>

int bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch(
    bx_ntvdm_boot_namespace_provider_v1 *provider,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint32_t released_slots;
    if (result == 0 || !bx_ntvdm_boot_namespace_provider_v1_valid(provider) ||
        !bx_ntvdm_bop_ingress_v1_valid(ingress) ||
        !bx_ntvdm_bop_provider_selection_v1_valid(selection) ||
        !bx_ntvdm_dem_plane_v1_valid(plane) ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    if (ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
        ingress->family != BX_NTVDM_BOP_FAMILY_DEM || ingress->service != 60u ||
        selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED ||
        selection->provider_family != BX_NTVDM_BOP_PROVIDER_DEM ||
        selection->precedence !=
            BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT ||
        plane->service != ingress->service ||
        /* demTerminatePDB is implemented by demsrch.c.  It is a MISC
         * lifecycle notification that releases per-PDB search state, not a
         * namespace selector exception. */
        plane->component != BX_NTVDM_DEM_COMPONENT_MISC ||
        plane->disposition != BX_NTVDM_DEM_PLANE_DEFERRED ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        event->fault_rip > UINT64_MAX - 4u) return 0;
    if (!bx_ntvdm_boot_namespace_provider_v1_terminate_pdb(provider,
            (uint16_t)cpu_before->ebx, &released_slots)) return 0;
    (void)released_slots;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u);
}
