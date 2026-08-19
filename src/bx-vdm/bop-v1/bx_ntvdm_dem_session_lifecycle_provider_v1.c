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
    uint32_t released_search_slots;
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
        (ingress->service != 0x36u && ingress->service != 0x3cu) ||
        selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED ||
        selection->provider_family != BX_NTVDM_BOP_PROVIDER_DEM ||
        selection->precedence !=
            BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT ||
        plane->service != ingress->service ||
        (ingress->service == 0x36u &&
            (plane->component != BX_NTVDM_DEM_COMPONENT_MISC ||
             plane->disposition != BX_NTVDM_DEM_PLANE_DEFERRED)) ||
        (ingress->service == 0x3cu &&
            (plane->component != BX_NTVDM_DEM_COMPONENT_MISC ||
             plane->disposition != BX_NTVDM_DEM_PLANE_DEFERRED)) ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        event->fault_rip > UINT64_MAX - 4u) return 0;
    /* demEntryDosApp only calls VDDCreateUserHook outside this no-VDD CLI
     * profile. Its retained result contract is the original void return. */
    if (ingress->service == 0x36u)
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u);
    /* SVC_PDBTERMINATE precedes DOS_ABORT.  OpenNT's abort path later walks
     * the guest JFT/SFT and emits ordinary SVC_DEMCLOSE calls.  Our opaque
     * Direct token occupies the historical SFT NT-handle position, so it
     * must remain live here for that later close; treating its diagnostic
     * PDB copy as a second close authority would forge a stale handle.
     *
     * demTerminatePDB itself owns only its per-PSP FindFirst/FindNext state
     * in this no-VDD/no-device CLI profile.  `whole_provider` remains part
     * of the package ABI but intentionally has no lifecycle action here. */
    (void)whole_provider;
    pdb = (uint16_t)cpu_before->ebx;
    if (!bx_ntvdm_boot_namespace_provider_v1_terminate_pdb(provider, pdb,
            &released_search_slots)) return 0;
    (void)released_search_slots;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u);
}
