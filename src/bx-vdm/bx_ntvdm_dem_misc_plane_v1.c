#include "bx_ntvdm_dem_misc_plane_v1.h"

#include "bx_ntvdm_dem_debug_service.h"
#include "bx_ntvdm_dem_plane_v1.h"

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
    default: return 0;
    }
}
