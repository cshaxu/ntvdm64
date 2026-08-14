#ifndef BX_NTVDM_DEM_CURRENT_DIR_SERVICE_V1_H
#define BX_NTVDM_DEM_CURRENT_DIR_SERVICE_V1_H

#include "bx_ntvdm_guest_gather_read_action_v1.h"
#include "bx_ntvdm_host_drive_policy.h"
#include "bx_ntvdm_multi_write_transaction.h"

int bx_ntvdm_dem_current_dir_service_v1_prepare(
    const bx_ntvdm_host_drive_snapshot_v1 *drives,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_dem_current_dir_service_v1_complete(
    const bx_ntvdm_host_drive_snapshot_v1 *drives, uint8_t service,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD]);

#endif
