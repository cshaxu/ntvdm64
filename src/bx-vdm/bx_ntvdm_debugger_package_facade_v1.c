#include "bx_ntvdm_debugger_package_facade_v1.h"

int bx_ntvdm_debugger_package_facade_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_system_plane_record_v1 plane;

    if (result == 0 || !bx_ntvdm_bop_ingress_v1_valid(ingress) ||
        !bx_ntvdm_bop_provider_selection_v1_valid(selection) ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    if (!bx_ntvdm_system_plane_v1_classify(ingress, selection, &plane) ||
        plane.provider_family != BX_NTVDM_BOP_PROVIDER_DEBUGGER ||
        plane.disposition != BX_NTVDM_SYSTEM_DEFERRED ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION ||
        event->vector != 6u) return 0;
    return bx_ntvdm_cpu_result_v2_stop(result);
}
