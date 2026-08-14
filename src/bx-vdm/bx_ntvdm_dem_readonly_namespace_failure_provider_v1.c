#include "bx_ntvdm_dem_readonly_namespace_failure_provider_v1.h"

#define BX_NTVDM_DEM_ERROR_ACCESS_DENIED 5u
#define BX_NTVDM_DEM_ERROR_INVALID_HANDLE 6u

static int readonly_terminal(uint8_t service)
{
    switch (service) {
    /* demfile.c: chmod, create, delete, rename, create-new. */
    case 0x01u: case 0x03u: case 0x05u: case 0x08u: case 0x17u: case 0x22u:
    /* demdir.c: create/delete directory.  18 is owned by the separately
     * admitted immutable-root current-directory capability. */
    case 0x04u: case 0x06u:
    /* demhndl.c: demCommit ignores FlushFileBuffers failure and resumes with
     * CF clear.  The contained profile has no writable host handle, but that
     * does not change this source-defined terminal result. */
    case 0x27u:
    /* demPipeFileDataEOF/demPipeFileEOF require the COMMAND pipe table, a
     * projected NT read handle and synchronization objects.  This CLI profile
     * deliberately creates no sf_nt_pipe_in SFT, so a synthetic call has no
     * projected handle rather than an ambient host pipe. */
    case 0x47u: case 0x48u:
        return 1;
    default:
        return 0;
    }
}

int bx_ntvdm_dem_readonly_namespace_failure_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_package_route_v1 *route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (!ingress || !selection || !route || !event || !cpu_before || !window ||
        !result || !bx_ntvdm_bop_ingress_v1_valid(ingress) ||
        !bx_ntvdm_bop_provider_selection_v1_valid(selection) ||
        !bx_ntvdm_dem_package_route_v1_valid(route) ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) ||
        ingress->family != BX_NTVDM_BOP_FAMILY_DEM ||
        ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
        selection->provider_family != BX_NTVDM_BOP_PROVIDER_DEM ||
        selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED ||
        route->plane.component != BX_NTVDM_DEM_COMPONENT_NAMESPACE ||
        route->plane.service != ingress->service || !readonly_terminal(ingress->service) ||
        event->vector != 6u || cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        event->fault_rip > UINT64_MAX - 4u || window->valid_bytes < 4u ||
        window->bytes[0] != 0xc4u || window->bytes[1] != 0xc4u ||
        window->bytes[2] != 0x50u || window->bytes[3] != ingress->service)
        return 0;
    if (ingress->service == 0x27u)
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 0);
    if (ingress->service == 0x47u || ingress->service == 0x48u)
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u,
                BX_NTVDM_DEM_ERROR_INVALID_HANDLE) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 1);
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u,
            BX_NTVDM_DEM_ERROR_ACCESS_DENIED) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1);
}
