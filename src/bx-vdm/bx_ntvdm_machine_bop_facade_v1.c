#include "bx_ntvdm_machine_bop_facade_v1.h"

int bx_ntvdm_machine_bop_facade_v1_classify(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection, uint32_t *route)
{
    if (route == 0 || !bx_ntvdm_bop_ingress_v1_valid(ingress) ||
        !bx_ntvdm_bop_provider_selection_v1_valid(selection) ||
        ingress->family != BX_NTVDM_BOP_FAMILY_TOP_LEVEL ||
        ingress->has_service != 0u ||
        selection->provider_family != BX_NTVDM_BOP_PROVIDER_TOP_LEVEL) return 0;
    *route = BX_NTVDM_MACHINE_BOP_NONE;
    switch (ingress->selector) {
    case 0x12u: case 0x15u:
        if (ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
            selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED) return 0;
        *route = BX_NTVDM_MACHINE_BOP_MEMORY; return 1;
    case 0x5fu:
        if (ingress->route != BX_NTVDM_BOP_ROUTE_EXPLICIT_UNAVAILABLE ||
            selection->disposition != BX_NTVDM_BOP_PROVIDER_EXPLICIT_UNAVAILABLE) return 0;
        *route = BX_NTVDM_MACHINE_BOP_HANDOFF; return 1;
    case 0x5cu: case 0x5du: case 0xfdu:
        if (ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
            selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED) return 0;
        *route = BX_NTVDM_MACHINE_BOP_DEFERRED_STOP; return 1;
    default: return 0;
    }
}

int bx_ntvdm_machine_bop_facade_v1_dispatch(uint32_t route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before, bx_ntvdm_cpu_result_v2 *result)
{
    if (result == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION || event->vector != 6u)
        return 0;
    return route == BX_NTVDM_MACHINE_BOP_DEFERRED_STOP ?
        bx_ntvdm_cpu_result_v2_stop(result) : 0;
}
