#include "bx_ntvdm_dem_fastio_provider_v1.h"

int bx_ntvdm_dem_fastio_provider_v1_dispatch(
    bx_ntvdm_boot_namespace_provider_v1 *space,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint8_t *payload,
    uint32_t payload_capacity, bx_ntvdm_bulk_result_transaction_v1 *transaction,
    bx_ntvdm_cpu_result_v2 *result)
{
    return bx_ntvdm_boot_namespace_provider_v1_fast_read(space, event, cpu, window,
        payload, payload_capacity, transaction, result);
}
