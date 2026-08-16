#include "bx_ntvdm_dem_fcb_io_route_partition_v1.h"

#include "bx_ntvdm_dem_fcb_handle_partition_v1.h"

#include <string.h>

static int matches(const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window)
{
    return boundary != 0 && cpu != 0 && window != 0 &&
        bx_ntvdm_exception_event_v1_valid(boundary) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        bx_ntvdm_instruction_window_v1_valid(window) && boundary->vector == 6u &&
        cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        boundary->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x50u && window->bytes[3] == 0x2fu;
}

static uint32_t token(const bx_ntvdm_cpu_state_v1 *cpu)
{ return ((cpu->eax & 0xffffu) << 16) | (cpu->ebp & 0xffffu); }

static int next_action_id(bx_ntvdm_dem_whole_provider_v1 *provider, uint32_t *out)
{
    if (provider == 0 || out == 0 || provider->next_action_id == 0u) return 0;
    *out = provider->next_action_id++;
    if (provider->next_action_id == 0u) provider->next_action_id = 1u;
    return 1;
}

static int action(bx_ntvdm_dem_whole_provider_v1 *provider, uint32_t kind,
    uint64_t address, const uint8_t *payload, uint32_t bytes,
    struct bx_ntvdm_mechanical_action_v1 *out)
{
    uint32_t id;
    if (provider == 0 || out == 0 || bytes == 0u ||
        bytes > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES ||
        address > UINT64_C(0x100000) - bytes || !next_action_id(provider, &id)) return 0;
    bx_ntvdm_mechanical_action_v1_clear(out);
    out->action_id = id;
    out->kind = kind;
    out->range_count = 1u;
    out->payload_bytes = bytes;
    out->ranges[0].physical_address = address;
    out->ranges[0].byte_count = bytes;
    if (kind == BX_NTVDM_MECHANICAL_ACTION_V1_WRITE && payload != 0)
        memcpy(out->payload, payload, bytes);
    return bx_ntvdm_mechanical_action_v1_valid(out);
}

int bx_ntvdm_dem_fcb_io_route_partition_v1_claims_request(
    const bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_cpu_state_v1 *cpu)
{
    HANDLE ignored; uint32_t kind;
    return provider != 0 && cpu != 0 && service == 0x2fu &&
        (bx_ntvdm_dem_file_session_v1_lookup(&provider->files, token(cpu), &ignored) ||
         (bx_ntvdm_dem_file_session_v1_token_kind(&provider->files, token(cpu), &kind) &&
          kind == BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_OVERLAY_FILE));
}

int bx_ntvdm_dem_fcb_io_route_partition_v1_dispatch(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint64_t dta_address,
    struct bx_ntvdm_mechanical_action_v1 *action_out,
    bx_ntvdm_cpu_result_v2 *result_out)
{
    uint32_t count, used = 0u;
    uint8_t bytes[BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES];
    if (provider == 0 || action_out == 0 || result_out == 0 ||
        !matches(boundary, cpu, window) ||
        !bx_ntvdm_dem_fcb_io_route_partition_v1_claims_request(provider, service, cpu) ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider)) return 0;
    count = (uint16_t)cpu->ecx;
    bx_ntvdm_mechanical_action_v1_clear(action_out);
    bx_ntvdm_cpu_result_v2_pass_through(result_out);
    if (count == 0u) return bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(provider,
        service, boundary, cpu, 0, 0, 0u, &used, result_out) &&
        bx_ntvdm_cpu_result_v2_valid(result_out);
    if ((cpu->ebx & 0xffffu) != 0u) {
        return bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(provider, service,
            boundary, cpu, 0, bytes, count, &used, result_out) && used <= count &&
            (used == 0u || action(provider, BX_NTVDM_MECHANICAL_ACTION_V1_WRITE,
                dta_address, bytes, used, action_out));
    }
    {
        bx_ntvdm_guest_range range;
        bx_ntvdm_guest_gather_read_action_v1 gather;
        range.address = dta_address;
        range.length = count;
        return bx_ntvdm_dem_whole_provider_v1_prepare_gather(provider, service,
            boundary, cpu, &range, 1u, &gather) &&
            action(provider, BX_NTVDM_MECHANICAL_ACTION_V1_READ, dta_address,
                0, count, action_out);
    }
}

int bx_ntvdm_dem_fcb_io_route_partition_v1_complete_write(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const struct bx_ntvdm_mechanical_action_v1 *read_action,
    bx_ntvdm_cpu_result_v2 *result_out)
{
    bx_ntvdm_guest_gather_read_action_v1 gather;
    uint8_t copied[BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES];
    uint32_t copied_bytes = 0u, used = 0u;
    if (provider == 0 || boundary == 0 || cpu == 0 || read_action == 0 ||
        result_out == 0 || service != 0x2fu ||
        !bx_ntvdm_dem_fcb_io_route_partition_v1_claims_request(provider, service, cpu) ||
        read_action->kind != BX_NTVDM_MECHANICAL_ACTION_V1_READ ||
        !bx_ntvdm_mechanical_action_v1_valid(read_action)) return 0;
    gather = provider->pending_gather;
    return bx_ntvdm_dem_whole_provider_v1_complete_gather(provider, service,
        boundary, cpu, &gather, read_action->payload, read_action->payload_bytes,
        copied, &copied_bytes) && copied_bytes == (uint16_t)cpu->ecx &&
        bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(provider, service,
            boundary, cpu, 0, copied, copied_bytes, &used, result_out) &&
        bx_ntvdm_cpu_result_v2_valid(result_out);
}
