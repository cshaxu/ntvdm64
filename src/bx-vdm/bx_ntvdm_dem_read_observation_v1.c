#include "bx_ntvdm_dem_read_observation_v1.h"

#include <string.h>

static uint32_t enabled;
static bx_ntvdm_dem_read_observation_v1 record;

void bx_ntvdm_dem_read_observation_v1_enable(uint32_t value)
{
    enabled = value == 1u;
    memset(&record, 0, sizeof(record));
}

int bx_ntvdm_dem_read_observation_v1_copy(bx_ntvdm_dem_read_observation_v1 *out)
{
    if (out == 0 || !enabled || !record.captured) return 0;
    *out = record;
    return 1;
}

void bx_ntvdm_dem_read_observation_v1_consider(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    uint8_t service, const uint8_t *payload, uint32_t payload_bytes)
{
    if (!enabled || record.captured || event == 0 || cpu_before == 0 ||
        payload == 0 || service != 0x16u || payload_bytes < 4u ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before)) return;
    ++record.seen_count;
    /* The original get_binary_type header fetch is exactly 0x3e bytes. */
    if ((uint16_t)cpu_before->ecx != 0x3eu) return;
    if (record.seen_count != 2u) return;
    record.captured = 1u;
    record.cs = cpu_before->cs;
    record.ds = cpu_before->ds;
    record.destination_offset = (uint16_t)cpu_before->edx;
    record.requested_bytes = (uint16_t)cpu_before->ecx;
    record.handle_ax = (uint16_t)cpu_before->eax;
    record.handle_bp = (uint16_t)cpu_before->ebp;
    record.eip = cpu_before->eip;
    memcpy(record.bytes, payload, sizeof(record.bytes));
}
