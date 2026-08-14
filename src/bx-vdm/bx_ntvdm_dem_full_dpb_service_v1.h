#ifndef BX_NTVDM_DEM_FULL_DPB_SERVICE_V1_H
#define BX_NTVDM_DEM_FULL_DPB_SERVICE_V1_H

#include "bx_ntvdm_host_volume_snapshot_v1.h"
#include "bx_ntvdm_multi_write_transaction.h"

/* Prepares OpenNT demGetDPB's no-BDS fake-DPB success branch.  It leaves the
 * historical branch's unassigned DPB bytes untouched. */
int bx_ntvdm_dem_full_dpb_service_v1_prepare(
    const bx_ntvdm_host_volume_snapshot_v1 *volumes,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD]);

#endif
