#include "bx_ntvdm_dem_cli_unavailable_provider_v1.h"

#include <stdint.h>

#define BX_NTVDM_DEM_ERROR_ACCESS_DENIED 5u
#define BX_NTVDM_DEM_ERROR_RETRY_UNAVAILABLE 0xffu

static int selected(const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_package_route_v1 *route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return bx_ntvdm_bop_ingress_v1_valid(ingress) &&
        bx_ntvdm_bop_provider_selection_v1_valid(selection) &&
        bx_ntvdm_dem_package_route_v1_valid(route) &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu_before) &&
        bx_ntvdm_instruction_window_v1_valid(window) &&
        ingress->route == BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED &&
        ingress->family == BX_NTVDM_BOP_FAMILY_DEM &&
        selection->disposition == BX_NTVDM_BOP_PROVIDER_DEFERRED &&
        selection->provider_family == BX_NTVDM_BOP_PROVIDER_DEM &&
        (route->disposition == BX_NTVDM_DEM_PACKAGE_EXPLICIT_SOURCE_FAILURE ||
         route->disposition == BX_NTVDM_DEM_PACKAGE_EXISTING_PROVIDER ||
         route->disposition == BX_NTVDM_DEM_PACKAGE_FASTREAD_COMPATIBILITY) &&
        route->plane.service == ingress->service &&
        route->plane.disposition == BX_NTVDM_DEM_PLANE_DEFERRED &&
        event->kind == BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION &&
        event->vector == 6u && event->fault_rip <= UINT64_MAX - 4u &&
        window->valid_bytes >= 4u && window->bytes[0] == 0xc4u &&
        window->bytes[1] == 0xc4u && window->bytes[2] == 0x50u &&
        window->bytes[3] == ingress->service;
}

int bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_package_route_v1 *route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint16_t error;
    if (result == 0 || !selected(ingress, selection, route, event, cpu_before,
            window)) return 0;
    if (cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL)
        return bx_ntvdm_cpu_result_v2_stop(result);
    /* demRetry has an explicit no-retry result in demerror.c.  demExitVDM is
     * a CLI lifecycle request rather than an emulated DOS service. */
    if (ingress->service == 0x33u)
        error = BX_NTVDM_DEM_ERROR_RETRY_UNAVAILABLE;
    else if (ingress->service == 0x3du)
        return bx_ntvdm_cpu_result_v2_stop(result);
    else
        error = BX_NTVDM_DEM_ERROR_ACCESS_DENIED;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, error) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1);
}
