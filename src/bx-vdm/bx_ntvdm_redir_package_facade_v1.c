#include "bx_ntvdm_redir_package_facade_v1.h"

#include <stdint.h>

int bx_ntvdm_redir_package_facade_v1_dispatch(
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
        plane.provider_family != BX_NTVDM_BOP_PROVIDER_REDIR ||
        plane.disposition != BX_NTVDM_SYSTEM_EXPLICIT_UNAVAILABLE ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION ||
        event->vector != 6u || event->fault_rip > UINT64_MAX - 4u) return 0;

    /* nt_bop.c/MS_bop_7 executes after the three-byte BOP selector and bumps
     * the service byte.  The common ingress copies from the BOP start, so the
     * exact corresponding continuation is fault RIP plus four bytes. */
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, 1u);
}
