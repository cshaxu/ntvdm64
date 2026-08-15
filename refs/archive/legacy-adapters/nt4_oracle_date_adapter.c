#include "nt4_oracle_date_adapter.h"

#include <string.h>

static const uint8_t nt4_date_trace_pattern[] = { 0xc4u, 0xc4u, 0x50u, 0x14u };

static int valid_snapshot(const owned_clock_snapshot *snapshot)
{
    return snapshot != NULL && snapshot->year >= 1980u && snapshot->month >= 1u &&
        snapshot->month <= 12u && snapshot->day >= 1u && snapshot->day <= 31u &&
        snapshot->weekday <= 6u;
}

static void nt4_date_handler(
    const owned_monitor_m0_state *state,
    uint64_t physical_fetch_address,
    const uint8_t *matched_bytes,
    size_t matched_byte_count,
    uintptr_t handler_id,
    void *context,
    owned_monitor_m1_handler_output *out_output)
{
    const nt4_oracle_date_adapter *adapter = (const nt4_oracle_date_adapter *)context;

    (void)physical_fetch_address;
    (void)matched_bytes;
    (void)matched_byte_count;
    (void)handler_id;

    if (!valid_snapshot(&adapter->snapshot)) {
        out_output->disposition = OWNED_MONITOR_M1_DISPOSITION_FAULT;
        return;
    }
    out_output->disposition = OWNED_MONITOR_M1_DISPOSITION_HANDLED_RESUME;
    out_output->patch.register_write_mask =
        OWNED_MONITOR_M1_PATCH_AX | OWNED_MONITOR_M1_PATCH_CX | OWNED_MONITOR_M1_PATCH_DX;
    out_output->patch.ax = (uint16_t)((state->ax & 0xff00u) | adapter->snapshot.weekday);
    out_output->patch.cx = adapter->snapshot.year;
    out_output->patch.dx = (uint16_t)((adapter->snapshot.month << 8u) | adapter->snapshot.day);
    out_output->patch.consumed_byte_count = (uint8_t)sizeof(nt4_date_trace_pattern);
}

int nt4_oracle_date_adapter_initialize(
    nt4_oracle_date_adapter *adapter,
    const owned_clock_snapshot *snapshot)
{
    if (adapter == NULL || !valid_snapshot(snapshot)) {
        return 0;
    }
    adapter->snapshot = *snapshot;
    return 1;
}

owned_monitor_m1_result nt4_oracle_date_adapter_register(
    owned_monitor_m1 *registry,
    nt4_oracle_date_adapter *adapter)
{
    if (registry == NULL || adapter == NULL) {
        return OWNED_MONITOR_M1_INVALID_ARGUMENT;
    }
    return owned_monitor_m1_register(registry, nt4_date_trace_pattern,
        sizeof(nt4_date_trace_pattern), 0u, nt4_date_handler, adapter);
}
