#ifndef BX_NTVDM_DEM_DPB_SERVICE_H
#define BX_NTVDM_DEM_DPB_SERVICE_H

#include <stdint.h>

#include "bx_ntvdm_multi_write_transaction.h"

/* Prepares the exact source-written fields of DEM 50:46.  It neither writes
 * guest memory nor accesses Bochs; the caller owns the returned payload. */
int bx_ntvdm_dem_dpb_service_v1_prepare(const uint8_t inventory_types[26],
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD]);

#endif
