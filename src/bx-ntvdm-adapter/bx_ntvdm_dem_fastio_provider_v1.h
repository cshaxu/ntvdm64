#ifndef BX_NTVDM_DEM_FASTIO_PROVIDER_V1_H
#define BX_NTVDM_DEM_FASTIO_PROVIDER_V1_H

#include "bx_ntvdm_boot_namespace_provider_v1.h"

/* Source-derived provider for the missing x86 FASTREAD composition.  It owns
 * only the read half of the family; FASTWRITE intentionally remains with the
 * original-unavailable DEM provider. */
int bx_ntvdm_dem_fastio_provider_v1_dispatch(
    bx_ntvdm_boot_namespace_provider_v1 *space,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint8_t *payload,
    uint32_t payload_capacity, bx_ntvdm_bulk_result_transaction_v1 *transaction,
    bx_ntvdm_cpu_result_v2 *result);

#endif
