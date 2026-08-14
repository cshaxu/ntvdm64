#ifndef BX_NTVDM_DEM_COMPUTER_NAME_SERVICE_V1_H
#define BX_NTVDM_DEM_COMPUTER_NAME_SERVICE_V1_H

#include "bx_ntvdm_multi_write_transaction.h"

/* Source-derived demGetComputerName branch for a CLI profile that does not
 * declare a stable guest machine identity. */
int bx_ntvdm_dem_computer_name_service_v1_prepare(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD]);

#endif
