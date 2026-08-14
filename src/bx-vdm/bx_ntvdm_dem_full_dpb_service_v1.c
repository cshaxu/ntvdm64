#include "bx_ntvdm_dem_full_dpb_service_v1.h"
#include "bx_ntvdm_dem_volume_provider_v1.h"

#define BX_NTVDM_DEM_DPB_BYTES 35u
#define BX_NTVDM_DEM_DPB_DRIVE_ADDR UINT32_C(0x04bc04bc)

static int selected(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window)
{
    return event && cpu && window && bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) && bx_ntvdm_instruction_window_v1_valid(window) &&
        event->vector == 6u && cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x50u && window->bytes[3] == 0x25u;
}

static int put8(bx_ntvdm_multi_write_v1 *writes, uint8_t *payload,
    uint64_t base, uint32_t offset, uint8_t value)
{
    uint64_t at = writes->payload_bytes;
    payload[at] = value;
    return bx_ntvdm_multi_write_v1_add(writes, base + offset, 1u, at);
}

static int put16(bx_ntvdm_multi_write_v1 *writes, uint8_t *payload,
    uint64_t base, uint32_t offset, uint16_t value)
{
    uint64_t at = writes->payload_bytes;
    payload[at] = (uint8_t)value; payload[at + 1u] = (uint8_t)(value >> 8);
    return bx_ntvdm_multi_write_v1_add(writes, base + offset, 2u, at);
}

static int put32(bx_ntvdm_multi_write_v1 *writes, uint8_t *payload,
    uint64_t base, uint32_t offset, uint32_t value)
{
    uint64_t at = writes->payload_bytes;
    payload[at] = (uint8_t)value; payload[at + 1u] = (uint8_t)(value >> 8);
    payload[at + 2u] = (uint8_t)(value >> 16); payload[at + 3u] = (uint8_t)(value >> 24);
    return bx_ntvdm_multi_write_v1_add(writes, base + offset, 4u, at);
}

int bx_ntvdm_dem_full_dpb_service_v1_prepare(
    const bx_ntvdm_host_volume_snapshot_v1 *volumes,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{
    const bx_ntvdm_host_volume_record_v1 *record;
    uint8_t drive;
    uint16_t sector_size, cluster_size, total_clusters, free_clusters;
    uint16_t shift = 0u;
    uint64_t base;

    if (!volumes || !transaction || !payload ||
        !bx_ntvdm_host_volume_snapshot_v1_valid(volumes) ||
        !selected(event, cpu, window)) return 0;
    drive = (uint8_t)cpu->eax;
    base = ((uint64_t)cpu->ds << 4) + (uint16_t)cpu->edi;
    if (drive >= 26u || base > UINT64_C(0x100000) - BX_NTVDM_DEM_DPB_BYTES ||
        (volumes->drives.admitted_mask & (UINT32_C(1) << drive)) == 0u ||
        (record = &volumes->volumes[drive])->available == 0u ||
        !bx_ntvdm_dem_volume_provider_v1_dos_geometry(record, &sector_size,
            &cluster_size, &total_clusters, &free_clusters)) return 0;
    while (((cluster_size >> shift) & 1u) == 0u) ++shift;
    bx_ntvdm_multi_write_transaction_v1_initialize(transaction, event, cpu);
    if (!put8(&transaction->writes, payload, base, 0u, drive) ||
        !put8(&transaction->writes, payload, base, 1u, drive) ||
        !put16(&transaction->writes, payload, base, 2u, sector_size) ||
        !put16(&transaction->writes, payload, base, 4u, (uint16_t)(cluster_size - 1u)) ||
        !put16(&transaction->writes, payload, base, 6u, shift) ||
        !put16(&transaction->writes, payload, base, 8u, 1u) ||
        !put8(&transaction->writes, payload, base, 10u, 2u) ||
        !put16(&transaction->writes, payload, base, 11u, 63u) ||
        !put16(&transaction->writes, payload, base, 15u, (uint16_t)(total_clusters + 1u)) ||
        !put16(&transaction->writes, payload, base, 17u, 512u) ||
        !put16(&transaction->writes, payload, base, 19u, 1u) ||
        !put32(&transaction->writes, payload, base, 21u, BX_NTVDM_DEM_DPB_DRIVE_ADDR) ||
        !put8(&transaction->writes, payload, base, 25u, 0xf8u) ||
        !put8(&transaction->writes, payload, base, 26u, 10u) ||
        !put32(&transaction->writes, payload, base, 27u, UINT32_MAX) ||
        !put16(&transaction->writes, payload, base, 33u, free_clusters) ||
        !bx_ntvdm_cpu_result_v2_resume(&transaction->result, event->fault_rip + 4u) ||
        !bx_ntvdm_cpu_delta_v1_set_gpr16(&transaction->result.cpu_delta, 0u, 0u) ||
        !bx_ntvdm_cpu_result_v2_set_cf(&transaction->result, 0)) return 0;
    return bx_ntvdm_multi_write_transaction_v1_preflight(transaction,
        UINT64_C(0x100000), transaction->writes.payload_bytes);
}
