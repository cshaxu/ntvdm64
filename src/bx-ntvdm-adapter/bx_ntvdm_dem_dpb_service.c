#include "bx_ntvdm_dem_dpb_service.h"

#define BX_NTVDM_DEM_DPB_BYTES 33u
#define BX_NTVDM_DEM_DPB_NEXT_OFFSET 25u
#define BX_NTVDM_DEM_DPB_ENTRY_WRITE_COUNT 2u
#define BX_NTVDM_DEM_DPB_BP_GPR_INDEX 6u

static int bx_ntvdm_dem_dpb_matches(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return event != 0 && cpu_before != 0 && window != 0 &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu_before) &&
        bx_ntvdm_instruction_window_v1_valid(window) && event->vector == 6u &&
        cpu_before->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        window->valid_bytes >= 4u && window->bytes[0] == 0xc4u &&
        window->bytes[1] == 0xc4u && window->bytes[2] == 0x50u &&
        window->bytes[3] == 0x46u;
}

static int bx_ntvdm_dem_dpb_physical(uint16_t segment, uint16_t offset,
    uint64_t *address)
{
    uint64_t value = ((uint64_t)segment << 4) + offset;
    if (address == 0 || value >= UINT64_C(0x100000))
        return 0;
    *address = value;
    return 1;
}

static int bx_ntvdm_dem_dpb_add(bx_ntvdm_multi_write_v1 *writes,
    uint8_t *payload, uint64_t base, uint8_t drive, uint32_t next)
{
    uint64_t offset = writes->payload_bytes;

    payload[offset] = drive;
    payload[offset + 1u] = drive;
    payload[offset + 2u] = (uint8_t)next;
    payload[offset + 3u] = (uint8_t)(next >> 8);
    payload[offset + 4u] = (uint8_t)(next >> 16);
    payload[offset + 5u] = (uint8_t)(next >> 24);
    return bx_ntvdm_multi_write_v1_add(writes, base, 2u, offset) &&
        bx_ntvdm_multi_write_v1_add(writes, base + BX_NTVDM_DEM_DPB_NEXT_OFFSET,
            4u, offset + 2u);
}

int bx_ntvdm_dem_dpb_service_v1_prepare(const uint8_t inventory_types[26],
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{
    uint16_t offset;
    uint16_t initial_offset;
    uint16_t segment;
    uint32_t drive;

    if (inventory_types == 0 || transaction == 0 || payload == 0 ||
        !bx_ntvdm_dem_dpb_matches(event, cpu_before, window) ||
        event->fault_rip > UINT64_MAX - 4u)
        return 0;
    for (drive = 0u; drive < 26u; ++drive) {
        if (inventory_types[drive] > 6u)
            return 0;
    }
    initial_offset = (uint16_t)cpu_before->ebp;
    offset = initial_offset;
    segment = cpu_before->es;
    bx_ntvdm_multi_write_transaction_v1_initialize(transaction, event,
        cpu_before);
    for (drive = 0u; drive < 26u; ++drive) {
        uint64_t base;
        uint16_t next_offset;
        uint32_t next;

        if (inventory_types[drive] != 2u && inventory_types[drive] != 3u)
            continue;
        if (offset > UINT16_MAX - BX_NTVDM_DEM_DPB_BYTES ||
            (next_offset = (uint16_t)(offset + BX_NTVDM_DEM_DPB_BYTES)) ==
                UINT16_MAX ||
            !bx_ntvdm_dem_dpb_physical(segment, offset, &base))
            return 0;
        next = ((uint32_t)segment << 16) | next_offset;
        if (!bx_ntvdm_dem_dpb_add(&transaction->writes, payload, base,
                (uint8_t)drive, next))
            return 0;
        offset = next_offset;
    }
    if (offset != initial_offset) {
        uint64_t last_next = transaction->writes.write_count - 1u;
        uint64_t payload_offset = transaction->writes.writes[last_next].payload_offset;

        payload[payload_offset] = 0xffu;
        payload[payload_offset + 1u] = 0xffu;
        payload[payload_offset + 2u] = 0xffu;
        payload[payload_offset + 3u] = 0xffu;
    }
    if (!bx_ntvdm_cpu_result_v2_resume(&transaction->result,
            event->fault_rip + 4u) ||
        !bx_ntvdm_cpu_delta_v1_set_gpr16(&transaction->result.cpu_delta,
            BX_NTVDM_DEM_DPB_BP_GPR_INDEX, offset))
        return 0;
    if (transaction->writes.write_count == 0u)
        return bx_ntvdm_cpu_result_v2_valid(&transaction->result);
    return bx_ntvdm_multi_write_transaction_v1_preflight(transaction,
        UINT64_C(0x100000), transaction->writes.payload_bytes);
}
