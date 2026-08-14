#include "bx_ntvdm_dem_misc_plane_v1.h"

#include "bx_ntvdm_dem_debug_service.h"
#include "bx_ntvdm_dem_plane_v1.h"
#include "bx_ntvdm_dem_system_symbol_service.h"

static int cli_noop_service(uint32_t service)
{
    switch (service) {
    /* demmisc.c: no debugger, no WOW, no physical floppy product shell. */
    case 0x1du: case 0x23u: case 0x34u: case 0x35u: case 0x36u:
    case 0x37u: case 0x38u: case 0x39u: case 0x3au: case 0x3eu:
        return 1;
    default:
        return 0;
    }
}

int bx_ntvdm_dem_misc_plane_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result)
{
    bx_ntvdm_dem_plane_record_v1 record;
    if (!ingress || !selection || !event || !cpu_before || !window || !result ||
        !bx_ntvdm_dem_plane_v1_classify(ingress, selection, &record) ||
        record.component != BX_NTVDM_DEM_COMPONENT_MISC ||
        record.disposition != BX_NTVDM_DEM_PLANE_DEFERRED)
        return 0;

    /* Keep each source-derived endpoint inside its original DEM component.
     * More miscellaneous endpoints can be admitted here only with their own
     * source contract; composition never dispatches a raw service number. */
    switch (record.service) {
    case 0x3bu: return bx_ntvdm_dem_debug_service_v1_dispatch(event,
        cpu_before, window, result);
    case 0x45u: return bx_ntvdm_dem_system_symbol_service_v1_dispatch(event,
        cpu_before, window, result);
    default:
        if (!cli_noop_service(record.service) || event->vector != 6u ||
            cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
            event->fault_rip > UINT64_MAX - 4u)
            return 0;
        return bx_ntvdm_exception_result_v1_resume(result,
            event->fault_rip + 4u);
    }
}
