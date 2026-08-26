#include "bulk_result_transaction.h"

#include <string.h>

void runtime_bulk_result_transaction_initialize(
    runtime_bulk_result_transaction *value,
    const runtime_exception_event *boundary,
    const runtime_cpu_state *cpu_before)
{
    if (value == 0) return;
    memset(value, 0, sizeof(*value));
    value->magic = RUNTIME_BULK_RESULT_TRANSACTION_MAGIC;
    value->abi_version = RUNTIME_BULK_RESULT_TRANSACTION_VERSION;
    value->struct_bytes = sizeof(*value);
    if (boundary != 0) value->boundary = *boundary;
    if (cpu_before != 0) value->cpu_before = *cpu_before;
    runtime_cpu_result_pass_through(&value->result);
}

int runtime_bulk_result_transaction_prepare(
    runtime_bulk_result_transaction *value, uint64_t guest_physical_address,
    uint32_t payload_bytes, const runtime_cpu_result *result)
{
    if (value == 0 || result == 0 || payload_bytes >
        RUNTIME_BULK_RESULT_TRANSACTION_MAX_PAYLOAD ||
        (payload_bytes == 0u && guest_physical_address != 0u) ||
        (payload_bytes != 0u && guest_physical_address > UINT64_MAX -
            (uint64_t)payload_bytes)) return 0;
    value->guest_physical_address = guest_physical_address;
    value->payload_bytes = payload_bytes;
    value->reserved0 = 0u;
    value->result = *result;
    return 1;
}

int runtime_bulk_result_transaction_preflight(
    const runtime_bulk_result_transaction *value,
    uint64_t aperture_bytes, uint64_t supplied_payload_bytes)
{
    if (value == 0 || value->magic != RUNTIME_BULK_RESULT_TRANSACTION_MAGIC ||
        value->abi_version != RUNTIME_BULK_RESULT_TRANSACTION_VERSION ||
        value->struct_bytes != sizeof(*value) || value->flags != 0u ||
        value->reserved0 != 0u || !runtime_exception_event_valid(
            &value->boundary) || !runtime_cpu_state_valid(
            &value->cpu_before) || !runtime_cpu_result_valid(
            &value->result) || value->result.disposition !=
            RUNTIME_CPU_RESULT_RESUME || value->result.resume_rip <=
            value->boundary.fault_rip || value->payload_bytes !=
            supplied_payload_bytes || value->payload_bytes >
            RUNTIME_BULK_RESULT_TRANSACTION_MAX_PAYLOAD) return 0;
    if (value->payload_bytes == 0u)
        return value->guest_physical_address == 0u;
    return value->guest_physical_address < aperture_bytes &&
        (uint64_t)value->payload_bytes <= aperture_bytes -
            value->guest_physical_address;
}
