#include "bx_ntvdm_redir_unavailable_provider_v1.h"

#include <stdint.h>

int bx_ntvdm_redir_unavailable_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (result == 0 || !bx_ntvdm_bop_ingress_v1_valid(ingress) ||
        !bx_ntvdm_bop_provider_selection_v1_valid(selection) ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    if (ingress->route != BX_NTVDM_BOP_ROUTE_EXPLICIT_UNAVAILABLE ||
        ingress->family != BX_NTVDM_BOP_FAMILY_TOP_LEVEL ||
        ingress->selector != 0x57u || ingress->has_service != 1u ||
        selection->disposition != BX_NTVDM_BOP_PROVIDER_EXPLICIT_UNAVAILABLE ||
        selection->provider_family != BX_NTVDM_BOP_PROVIDER_TOP_LEVEL ||
        selection->precedence !=
            BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_FAILURE_OR_DEFERRED ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION ||
        event->vector != 6u || event->fault_rip > UINT64_MAX - 3u)
        return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 3u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, 1u);
}
