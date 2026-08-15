#include "bx_ntvdm_dem_namespace_route_partition_v1.h"

#include <string.h>

#define DEM_PATH_BYTES BX_NTVDM_DEM_PATH_V1_MAX_OEM_BYTES

static int physical(uint16_t segment, uint16_t offset, uint32_t bytes,
    uint64_t *out)
{
    uint64_t address = ((uint64_t)segment << 4) + offset;
    if (out == 0 || bytes == 0u || address > UINT64_C(0x100000) - bytes) return 0;
    *out = address;
    return 1;
}

static int matches(uint8_t service, const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window)
{
    return boundary != 0 && cpu != 0 && window != 0 &&
        bx_ntvdm_exception_event_v1_valid(boundary) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        bx_ntvdm_instruction_window_v1_valid(window) && boundary->vector == 6u &&
        cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        boundary->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x50u && window->bytes[3] == service;
}

static int action_from_gather(bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_guest_gather_read_action_v1 *gather,
    struct bx_ntvdm_mechanical_action_v1 *out)
{
    uint32_t index, offset = 0u, id;
    if (provider == 0 || gather == 0 || out == 0 ||
        gather->disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        !bx_ntvdm_guest_gather_read_action_v1_valid(gather) ||
        gather->range_count == 0u ||
        gather->range_count > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES ||
        gather->total_bytes > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES ||
        provider->next_action_id == 0u) return 0;
    id = provider->next_action_id++;
    if (provider->next_action_id == 0u) provider->next_action_id = 1u;
    bx_ntvdm_mechanical_action_v1_clear(out);
    out->action_id = id;
    out->kind = BX_NTVDM_MECHANICAL_ACTION_V1_READ;
    out->range_count = gather->range_count;
    out->payload_bytes = gather->total_bytes;
    for (index = 0u; index < gather->range_count; ++index) {
        if (gather->ranges[index].length > UINT32_MAX ||
            offset > out->payload_bytes - (uint32_t)gather->ranges[index].length)
            return 0;
        out->ranges[index].physical_address = gather->ranges[index].address;
        out->ranges[index].byte_count = (uint32_t)gather->ranges[index].length;
        out->ranges[index].payload_offset = offset;
        offset += out->ranges[index].byte_count;
    }
    return offset == out->payload_bytes && bx_ntvdm_mechanical_action_v1_valid(out);
}

int bx_ntvdm_dem_namespace_route_partition_v1_owns_service(uint8_t service)
{ return bx_ntvdm_dem_namespace_partition_v1_owns_service(service); }

int bx_ntvdm_dem_namespace_route_partition_v1_prepare(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    struct bx_ntvdm_mechanical_action_v1 *action_out)
{
    bx_ntvdm_guest_range ranges[2];
    bx_ntvdm_guest_gather_read_action_v1 gather;
    uint32_t count = 1u;
    uint16_t first_offset;
    if (provider == 0 || action_out == 0 || !matches(service, boundary, cpu, window) ||
        !bx_ntvdm_dem_namespace_route_partition_v1_owns_service(service) ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider)) return 0;
    first_offset = (service == 0x03u || service == 0x12u || service == 0x22u ||
        service == 0x44u) ? (uint16_t)cpu->esi : (uint16_t)cpu->edx;
    if (!physical(cpu->ds, first_offset, DEM_PATH_BYTES, &ranges[0].address)) return 0;
    ranges[0].length = DEM_PATH_BYTES;
    if (service == 0x17u) {
        if (!physical(cpu->es, (uint16_t)cpu->edi, DEM_PATH_BYTES, &ranges[1].address))
            return 0;
        ranges[1].length = DEM_PATH_BYTES;
        count = 2u;
    }
    return bx_ntvdm_dem_whole_provider_v1_prepare_gather(provider, service,
        boundary, cpu, ranges, count, &gather) &&
        action_from_gather(provider, &gather, action_out);
}

int bx_ntvdm_dem_namespace_route_partition_v1_complete(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const struct bx_ntvdm_mechanical_action_v1 *read_action,
    bx_ntvdm_cpu_result_v2 *result_out)
{
    bx_ntvdm_guest_gather_read_action_v1 gather;
    uint8_t copied[BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES];
    char first[DEM_PATH_BYTES], second[DEM_PATH_BYTES];
    uint32_t copied_bytes = 0u;
    uint32_t expected = service == 0x17u ? 2u * DEM_PATH_BYTES : DEM_PATH_BYTES;
    if (provider == 0 || boundary == 0 || cpu == 0 || read_action == 0 ||
        result_out == 0 || !bx_ntvdm_dem_namespace_route_partition_v1_owns_service(service) ||
        read_action->kind != BX_NTVDM_MECHANICAL_ACTION_V1_READ ||
        !bx_ntvdm_mechanical_action_v1_valid(read_action) ||
        read_action->payload_bytes != expected) return 0;
    gather = provider->pending_gather;
    if (!bx_ntvdm_dem_whole_provider_v1_complete_gather(provider, service,
            boundary, cpu, &gather, read_action->payload, read_action->payload_bytes,
            copied, &copied_bytes) || copied_bytes != expected) return 0;
    memcpy(first, copied, DEM_PATH_BYTES);
    if (memchr(first, '\0', sizeof(first)) == 0) return 0;
    if (service == 0x17u) {
        memcpy(second, copied + DEM_PATH_BYTES, DEM_PATH_BYTES);
        if (memchr(second, '\0', sizeof(second)) == 0) return 0;
        return bx_ntvdm_dem_namespace_partition_v1_dispatch(provider, service,
            boundary, cpu, first, second, result_out) && bx_ntvdm_cpu_result_v2_valid(result_out);
    }
    return bx_ntvdm_dem_namespace_partition_v1_dispatch(provider, service,
        boundary, cpu, first, 0, result_out) && bx_ntvdm_cpu_result_v2_valid(result_out);
}
