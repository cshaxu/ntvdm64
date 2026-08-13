#include "bx_ntvdm_bulk_result_transaction.h"

#include <string.h>

void bx_ntvdm_bulk_result_transaction_v1_initialize(
    bx_ntvdm_bulk_result_transaction_v1 *value,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before)
{
    if (value == 0) return;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_BULK_RESULT_TRANSACTION_V1_MAGIC;
    value->abi_version = BX_NTVDM_BULK_RESULT_TRANSACTION_V1_VERSION;
    value->struct_bytes = sizeof(*value);
    if (boundary != 0) value->boundary = *boundary;
    if (cpu_before != 0) value->cpu_before = *cpu_before;
    bx_ntvdm_cpu_result_v2_pass_through(&value->result);
}

int bx_ntvdm_bulk_result_transaction_v1_prepare(
    bx_ntvdm_bulk_result_transaction_v1 *value, uint64_t guest_physical_address,
    uint32_t payload_bytes, const bx_ntvdm_cpu_result_v2 *result)
{
    if (value == 0 || result == 0 || payload_bytes >
        BX_NTVDM_BULK_RESULT_TRANSACTION_V1_MAX_PAYLOAD ||
        (payload_bytes == 0u && guest_physical_address != 0u) ||
        (payload_bytes != 0u && guest_physical_address > UINT64_MAX -
            (uint64_t)payload_bytes)) return 0;
    value->guest_physical_address = guest_physical_address;
    value->payload_bytes = payload_bytes;
    value->reserved0 = 0u;
    value->result = *result;
    return 1;
}

int bx_ntvdm_bulk_result_transaction_v1_preflight(
    const bx_ntvdm_bulk_result_transaction_v1 *value,
    uint64_t aperture_bytes, uint64_t supplied_payload_bytes)
{
    if (value == 0 || value->magic != BX_NTVDM_BULK_RESULT_TRANSACTION_V1_MAGIC ||
        value->abi_version != BX_NTVDM_BULK_RESULT_TRANSACTION_V1_VERSION ||
        value->struct_bytes != sizeof(*value) || value->flags != 0u ||
        value->reserved0 != 0u || !bx_ntvdm_exception_event_v1_valid(
            &value->boundary) || !bx_ntvdm_cpu_state_v1_valid(
            &value->cpu_before) || !bx_ntvdm_cpu_result_v2_valid(
            &value->result) || value->result.disposition !=
            BX_NTVDM_CPU_RESULT_V2_RESUME || value->result.resume_rip <=
            value->boundary.fault_rip || value->payload_bytes !=
            supplied_payload_bytes || value->payload_bytes >
            BX_NTVDM_BULK_RESULT_TRANSACTION_V1_MAX_PAYLOAD) return 0;
    if (value->payload_bytes == 0u)
        return value->guest_physical_address == 0u;
    return value->guest_physical_address < aperture_bytes &&
        (uint64_t)value->payload_bytes <= aperture_bytes -
            value->guest_physical_address;
}
