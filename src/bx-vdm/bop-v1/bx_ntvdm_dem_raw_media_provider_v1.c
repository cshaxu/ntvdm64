#include "bx_ntvdm_dem_raw_media_provider_v1.h"

#define BX_NTVDM_DEM_IOCTL 0x21u
#define BX_NTVDM_DEM_ABS_READ 0x29u
#define BX_NTVDM_DEM_ABS_WRITE 0x2au
#define BX_NTVDM_DEM_ERROR_INVALID_FUNCTION 1u
#define BX_NTVDM_DEM_ERROR_DRIVE_NOT_READY 21u

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
        ingress->family == BX_NTVDM_BOP_FAMILY_DEM &&
        ingress->route == BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED &&
        selection->provider_family == BX_NTVDM_BOP_PROVIDER_DEM &&
        selection->disposition == BX_NTVDM_BOP_PROVIDER_DEFERRED &&
        route->plane.component == BX_NTVDM_DEM_COMPONENT_RAW_MEDIA &&
        route->plane.service == ingress->service &&
        event->kind == BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION &&
        event->vector == 6u &&
        cpu_before->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x50u && window->bytes[3] == ingress->service;
}

int bx_ntvdm_dem_raw_media_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_package_route_v1 *route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint16_t error;
    if (!result || !selected(ingress, selection, route, event, cpu_before,
            window)) return 0;
    if (ingress->service == BX_NTVDM_DEM_IOCTL)
        error = BX_NTVDM_DEM_ERROR_INVALID_FUNCTION;
    else if (ingress->service == BX_NTVDM_DEM_ABS_READ ||
        ingress->service == BX_NTVDM_DEM_ABS_WRITE)
        error = BX_NTVDM_DEM_ERROR_DRIVE_NOT_READY;
    else
        return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, error) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1);
}
