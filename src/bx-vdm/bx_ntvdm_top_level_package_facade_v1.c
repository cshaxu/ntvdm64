#include "bx_ntvdm_top_level_package_facade_v1.h"

#include <stdint.h>

int bx_ntvdm_top_level_package_facade_v1_classify(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    uint32_t *route)
{
    if (route == 0 || !bx_ntvdm_bop_ingress_v1_valid(ingress) ||
        !bx_ntvdm_bop_provider_selection_v1_valid(selection) ||
        ingress->has_service != 0u) return 0;
    *route = BX_NTVDM_TOP_LEVEL_PACKAGE_NONE;
    switch (ingress->selector) {
    case 0x51u:
        /* BOP 51 retains its original WOW ownership even though this grouped
         * facade supplies the CLI terminal disposition. */
        if (ingress->family != BX_NTVDM_BOP_FAMILY_WOW ||
            selection->provider_family != BX_NTVDM_BOP_PROVIDER_WOW ||
            ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
            selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED) return 0;
        *route = BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL; break;
    case 0xfeu:
        if (ingress->family != BX_NTVDM_BOP_FAMILY_TOP_LEVEL ||
            selection->provider_family != BX_NTVDM_BOP_PROVIDER_TOP_LEVEL ||
            ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
            selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED) return 0;
        *route = BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL; break;
    case 0x59u: case 0x5bu:
        if (ingress->family != BX_NTVDM_BOP_FAMILY_TOP_LEVEL ||
            selection->provider_family != BX_NTVDM_BOP_PROVIDER_TOP_LEVEL ||
            ingress->route != BX_NTVDM_BOP_ROUTE_EXPLICIT_UNAVAILABLE ||
            selection->disposition != BX_NTVDM_BOP_PROVIDER_EXPLICIT_UNAVAILABLE) return 0;
        *route = BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL; break;
    case 0x5au:
        if (ingress->family != BX_NTVDM_BOP_FAMILY_TOP_LEVEL ||
            selection->provider_family != BX_NTVDM_BOP_PROVIDER_TOP_LEVEL ||
            ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
            selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED) return 0;
        *route = BX_NTVDM_TOP_LEVEL_PACKAGE_IDLE; break;
    case 0x5eu:
        if (ingress->family != BX_NTVDM_BOP_FAMILY_TOP_LEVEL ||
            selection->provider_family != BX_NTVDM_BOP_PROVIDER_TOP_LEVEL ||
            ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
            selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED) return 0;
        *route = BX_NTVDM_TOP_LEVEL_PACKAGE_CONFIG; break;
    case 0x5fu:
        if (ingress->family != BX_NTVDM_BOP_FAMILY_TOP_LEVEL ||
            selection->provider_family != BX_NTVDM_BOP_PROVIDER_TOP_LEVEL ||
            ingress->route != BX_NTVDM_BOP_ROUTE_EXPLICIT_UNAVAILABLE ||
            selection->disposition != BX_NTVDM_BOP_PROVIDER_EXPLICIT_UNAVAILABLE) return 0;
        *route = BX_NTVDM_TOP_LEVEL_PACKAGE_KEYBOARD; break;
    default: return 0;
    }
    return 1;
}

int bx_ntvdm_top_level_package_facade_v1_dispatch(uint32_t route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (result == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION ||
        event->vector != 6u) return 0;
    if (route == BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL)
        return bx_ntvdm_cpu_result_v2_stop(result);
    if (route != BX_NTVDM_TOP_LEVEL_PACKAGE_IDLE ||
        event->fault_rip > UINT64_MAX - 3u) return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 3u);
}
