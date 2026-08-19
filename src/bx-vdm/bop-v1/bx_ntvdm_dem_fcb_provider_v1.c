#include "bx_ntvdm_dem_fcb_provider_v1.h"

#include <time.h>

#define BX_NTVDM_DEM_ERROR_ACCESS_DENIED 5u

static int selected(const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_package_route_v1 *route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return ingress && selection && route && event && cpu_before && window &&
        bx_ntvdm_bop_ingress_v1_valid(ingress) &&
        bx_ntvdm_bop_provider_selection_v1_valid(selection) &&
        bx_ntvdm_dem_package_route_v1_valid(route) &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu_before) &&
        bx_ntvdm_instruction_window_v1_valid(window) &&
        ingress->route == BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED &&
        ingress->family == BX_NTVDM_BOP_FAMILY_DEM &&
        selection->disposition == BX_NTVDM_BOP_PROVIDER_DEFERRED &&
        selection->provider_family == BX_NTVDM_BOP_PROVIDER_DEM &&
        route->plane.component == BX_NTVDM_DEM_COMPONENT_FCB &&
        route->plane.service == ingress->service && event->vector == 6u &&
        cpu_before->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x50u && window->bytes[3] == ingress->service;
}

static int fcb_date16(const bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_result_v2 *result)
{
    time_t now = time(0);
    struct tm local;
    uint16_t date, clock;
    if (now == (time_t)-1 || localtime_s(&local, &now) != 0) return 0;
    date = (uint16_t)(((uint16_t)(local.tm_year + 1900) << 9) |
        ((uint16_t)(local.tm_mon + 1) << 5) | (uint16_t)local.tm_mday);
    clock = (uint16_t)(((uint16_t)local.tm_hour << 11) |
        ((uint16_t)local.tm_min << 5) | ((uint16_t)local.tm_sec / 2u));
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, date) &&
        /* demfcb.c demDate16 returns its packed time in DX, not BX. */
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, clock);
}

/* These identities are one FCB host-handle family in demfcb.c.  The profile
 * intentionally has neither writable files nor a projected NT-handle table;
 * name each terminal member here instead of treating an arbitrary FCB route
 * as an interchangeable package fallback.  0A/0C are handled first by the
 * bounded search provider and 2E/30 have the source-specific branches below.
 */
static int fcb_requires_unavailable_handle_backend(uint8_t service)
{
    switch (service) {
    case 0x07u: /* demDeleteFCB */
    case 0x20u: /* demRenameFCB */
    case 0x2cu: /* demCreateFCB */
    case 0x2du: /* demOpenFCB */
    case 0x2fu: /* demFCBIO */
    case 0x31u: /* demGetFileInfo */
        return 1;
    default:
        return 0;
    }
}

int bx_ntvdm_dem_fcb_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_package_route_v1 *route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (!result || !selected(ingress, selection, route, event, cpu_before, window))
        return 0;
    /* demCloseFCB accepts a null historical handle; demDate16 always reports
     * the local DOS-format clock. */
    if (ingress->service == 0x2eu &&
        ((cpu_before->eax & 0xffffu) | (cpu_before->esi & 0xffffu)) == 0u)
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 0);
    if (ingress->service == 0x30u) return fcb_date16(event, result);
    if (!fcb_requires_unavailable_handle_backend(ingress->service) &&
        ingress->service != 0x2eu && ingress->service != 0x0au &&
        ingress->service != 0x0cu) return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u,
            BX_NTVDM_DEM_ERROR_ACCESS_DENIED) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1);
}
