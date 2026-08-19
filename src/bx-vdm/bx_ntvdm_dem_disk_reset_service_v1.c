#include "bx_ntvdm_dem_disk_reset_service_v1.h"

#include <stdint.h>

int bx_ntvdm_dem_disk_reset_service_v1_prepare(
    uint64_t fd_access,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    uint32_t action_id,
    struct bx_ntvdm_mechanical_action_v1 *action,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (ingress == 0 || event == 0 || cpu == 0 || action == 0 || result == 0 ||
        action_id == 0 || ingress->family != BX_NTVDM_BOP_FAMILY_DEM ||
        ingress->service != 0x23u || event->vector != 6u ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        event->fault_rip > UINT64_MAX - 4u ||
        fd_access > UINT64_C(0x100000) - 2u) {
        return 0;
    }

    bx_ntvdm_mechanical_action_v1_clear(action);
    action->action_id = action_id;
    action->kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
    action->range_count = 1u;
    action->payload_bytes = 2u;
    action->ranges[0].physical_address = fd_access;
    action->ranges[0].byte_count = 2u;

    return bx_ntvdm_mechanical_action_v1_valid(action) &&
        bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_valid(result);
}