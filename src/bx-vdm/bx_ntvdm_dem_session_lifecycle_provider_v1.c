#include "bx_ntvdm_dem_session_lifecycle_provider_v1.h"

#include <stdint.h>

int bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch(
    bx_ntvdm_boot_namespace_provider_v1 *provider,
    bx_ntvdm_dem_whole_provider_v1 *whole_provider,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint32_t released_slots;
    uint16_t pdb;
    if (result == 0 || !bx_ntvdm_boot_namespace_provider_v1_valid(provider) ||
        !bx_ntvdm_bop_ingress_v1_valid(ingress) ||
        !bx_ntvdm_bop_provider_selection_v1_valid(selection) ||
        !bx_ntvdm_dem_plane_v1_valid(plane) ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    if (ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
        ingress->family != BX_NTVDM_BOP_FAMILY_DEM ||
        (ingress->service != 36u && ingress->service != 60u) ||
        selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED ||
        selection->provider_family != BX_NTVDM_BOP_PROVIDER_DEM ||
        selection->precedence !=
            BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT ||
        plane->service != ingress->service ||
        (ingress->service == 36u &&
            (plane->component != BX_NTVDM_DEM_COMPONENT_ORIGINAL_NOOP ||
             plane->disposition != BX_NTVDM_DEM_PLANE_ORIGINAL_NOOP)) ||
        (ingress->service == 60u &&
            (plane->component != BX_NTVDM_DEM_COMPONENT_MISC ||
             plane->disposition != BX_NTVDM_DEM_PLANE_DEFERRED)) ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        event->fault_rip > UINT64_MAX - 4u) return 0;
    /* demEntryDosApp only calls VDDCreateUserHook outside this no-VDD CLI
     * profile. Its retained result contract is the original void return. */
    if (ingress->service == 36u)
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u);
    pdb = (uint16_t)cpu_before->ebx;
    if (!bx_ntvdm_boot_namespace_provider_v1_terminate_pdb(provider, pdb,
            &released_slots)) return 0;
    /* Direct handles are the only current PDB-owned file resource. Readonly
     * startup tokens are session-scoped; Overlay/Virtual require their own
     * later lifecycle providers and never fall through to this cleanup. */
    if (whole_provider != 0 && !bx_ntvdm_dem_whole_provider_v1_valid(whole_provider))
        return 0;
    if (whole_provider != 0 && pdb != 0u &&
        !bx_ntvdm_dem_file_session_v1_release_owner(&whole_provider->files,
            pdb, &released_slots)) return 0;
    (void)released_slots;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u);
}
