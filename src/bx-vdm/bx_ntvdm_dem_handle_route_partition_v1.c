#include "bx_ntvdm_dem_handle_route_partition_v1.h"

#include "bx_ntvdm_dem_handle_partition_v1.h"
#include "bx_ntvdm_dem_overlay_handle_backend_v1.h"
#include "bx_ntvdm_dem_readonly_file_service.h"

static int physical(uint16_t segment, uint16_t offset, uint32_t bytes,
    uint64_t *out)
{
    uint64_t value = ((uint64_t)segment << 4) + offset;
    if (out == 0 || bytes == 0u || value > UINT64_C(0x100000) - bytes) return 0;
    *out = value;
    return 1;
}

static int matches(uint8_t service, const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window)
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

static int next_action_id(bx_ntvdm_dem_whole_provider_v1 *provider,
    uint32_t *out)
{
    if (provider == 0 || out == 0 || provider->next_action_id == 0u) return 0;
    *out = provider->next_action_id++;
    if (provider->next_action_id == 0u) provider->next_action_id = 1u;
    return 1;
}

static int read_action(bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_guest_gather_read_action_v1 *gather,
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    uint32_t id;
    if (provider == 0 || gather == 0 || action == 0 ||
        !bx_ntvdm_guest_gather_read_action_v1_valid(gather) ||
        gather->disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        gather->range_count != 1u || gather->total_bytes == 0u ||
        gather->total_bytes > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES ||
        gather->ranges[0].length != gather->total_bytes ||
        !next_action_id(provider, &id)) return 0;
    bx_ntvdm_mechanical_action_v1_clear(action);
    action->action_id = id;
    action->kind = BX_NTVDM_MECHANICAL_ACTION_V1_READ;
    action->range_count = 1u;
    action->payload_bytes = gather->total_bytes;
    action->ranges[0].physical_address = gather->ranges[0].address;
    action->ranges[0].byte_count = gather->total_bytes;
    return bx_ntvdm_mechanical_action_v1_valid(action);
}

static int write_action(bx_ntvdm_dem_whole_provider_v1 *provider,
    uint64_t address, const uint8_t *bytes, uint32_t byte_count,
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    uint32_t id;
    if (provider == 0 || bytes == 0 || action == 0 || byte_count == 0u ||
        address > UINT64_C(0x100000) - byte_count ||
        !next_action_id(provider, &id)) return 0;
    bx_ntvdm_mechanical_action_v1_clear(action);
    action->action_id = id;
    action->kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
    action->range_count = 1u;
    action->payload_bytes = byte_count;
    action->ranges[0].physical_address = address;
    action->ranges[0].byte_count = byte_count;
    memcpy(action->payload, bytes, byte_count);
    return bx_ntvdm_mechanical_action_v1_valid(action);
}

static int overlay_finish(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, uint16_t ax, int carry)
{
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        (!carry || bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax)) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, carry);
}

static uint32_t token(const bx_ntvdm_cpu_state_v1 *cpu)
{ return ((cpu->eax & 0xffffu) << 16) | (cpu->ebp & 0xffffu); }

static int readonly_token(const bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_cpu_state_v1 *cpu, uint32_t *backend_token_out)
{
    return provider != 0 && provider->startup_namespace != 0 && cpu != 0 &&
        bx_ntvdm_dem_file_session_v1_lookup_backend(&provider->files, token(cpu),
            BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_READONLY_NAMESPACE, backend_token_out) &&
        bx_ntvdm_readonly_namespace_v1_owns_token(provider->startup_namespace,
            *backend_token_out);
}

static int overlay_token(const bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_cpu_state_v1 *cpu)
{
    uint32_t ignored;
    return provider != 0 && cpu != 0 &&
        (bx_ntvdm_dem_file_session_v1_lookup_backend(&provider->files, token(cpu),
            BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_OVERLAY_FILE, &ignored) ||
         bx_ntvdm_dem_file_session_v1_lookup_backend(&provider->files, token(cpu),
            BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_VIRTUAL_FILE, &ignored));
}

static void substitute_token(bx_ntvdm_cpu_state_v1 *destination,
    const bx_ntvdm_cpu_state_v1 *source, uint32_t backend_token)
{
    *destination = *source;
    destination->eax = (destination->eax & UINT32_C(0xffff0000)) |
        (backend_token >> 16);
    destination->ebp = (destination->ebp & UINT32_C(0xffff0000)) |
        (backend_token & 0xffffu);
}

static int startup_overlay_dispatch(bx_ntvdm_dem_whole_provider_v1 *provider,
    uint8_t service, const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window,
    struct bx_ntvdm_mechanical_action_v1 *action_out,
    bx_ntvdm_cpu_result_v2 *result_out)
{
    bx_ntvdm_bulk_result_transaction_v1 transaction;
    uint8_t bytes[BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES];
    bx_ntvdm_cpu_state_v1 readonly_cpu;
    uint32_t backend_token;
    if (!readonly_token(provider, cpu, &backend_token)) return 0;
    substitute_token(&readonly_cpu, cpu, backend_token);
    if (service == 0x00u) return bx_ntvdm_dem_readonly_file_v1_seek(
        provider->startup_namespace, boundary, &readonly_cpu, window, result_out);
    if (service == 0x02u) {
        if (!bx_ntvdm_dem_readonly_file_v1_close(provider->startup_namespace,
                boundary, &readonly_cpu, window, result_out)) return 0;
        if (!bx_ntvdm_readonly_namespace_v1_owns_token(provider->startup_namespace,
                backend_token) && !bx_ntvdm_dem_file_session_v1_release_backend(
                    &provider->files, token(cpu),
                    BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_READONLY_NAMESPACE)) return 0;
        return 1;
    }
    if (service == 0x08u) return bx_ntvdm_dem_readonly_file_v1_file_times(
        provider->startup_namespace, boundary, &readonly_cpu, window, result_out);
    if (service == 0x1eu)
        return overlay_finish(boundary, result_out, 5u, 1);
    if (service == 0x27u)
        return overlay_finish(boundary, result_out, 0u, 0);
    if (service != 0x16u || !bx_ntvdm_dem_readonly_file_v1_read(
            provider->startup_namespace, boundary, &readonly_cpu, window, bytes, sizeof(bytes),
            &transaction, result_out)) return 0;
    if (transaction.magic == 0u) return 1;
    if (!bx_ntvdm_bulk_result_transaction_v1_preflight(&transaction,
            UINT64_C(0x100000), transaction.payload_bytes)) return 0;
    if (transaction.payload_bytes == 0u) return 1;
    return write_action(provider, transaction.guest_physical_address, bytes,
        transaction.payload_bytes, action_out);
}

static int overlay_error(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result)
{ return overlay_finish(boundary, result, 6u, 1); }

static int overlay_seek_if_requested(bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_cpu_state_v1 *cpu)
{
    uint32_t ignored;
    if ((cpu->eflags & 0x40u) != 0u) return 1;
    return bx_ntvdm_dem_overlay_handle_backend_v1_seek(&provider->files,
        &provider->overlay_files, token(cpu), (int32_t)(uint32_t)
        (((cpu->ebx & 0xffffu) << 16) | (cpu->esi & 0xffffu)),
        BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_BEGIN, &ignored);
}

static int overlay_handle_dispatch(bx_ntvdm_dem_whole_provider_v1 *provider,
    uint8_t service, const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu, struct bx_ntvdm_mechanical_action_v1 *action_out,
    bx_ntvdm_cpu_result_v2 *result_out)
{
    uint8_t bytes[BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES];
    uint32_t count = (uint16_t)cpu->ecx, value = 0u;
    uint64_t address;
    if (!overlay_token(provider, cpu)) return 0;
    if (service == 0x08u) {
        uint16_t time = (uint16_t)cpu->ecx, date = (uint16_t)cpu->edx;
        uint8_t option = (uint8_t)cpu->ebx;
        if (!bx_ntvdm_dem_overlay_handle_backend_v1_file_times(&provider->files,
                &provider->overlay_files, token(cpu), option, &time, &date))
            return overlay_error(boundary, result_out);
        return overlay_finish(boundary, result_out, 0u, 0) &&
            (option != 0u || (bx_ntvdm_cpu_delta_v1_set_gpr16(&result_out->cpu_delta, 1u, time) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result_out->cpu_delta, 2u, date)));
    }
    if (service == 0x00u) {
        uint8_t origin = (uint8_t)(cpu->ebx & 0xffu);
        if (origin > BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_END ||
            !bx_ntvdm_dem_overlay_handle_backend_v1_seek(&provider->files,
                &provider->overlay_files, token(cpu), (int32_t)(uint32_t)
                (((cpu->ecx & 0xffffu) << 16) | (cpu->edx & 0xffffu)), origin, &value))
            return overlay_error(boundary, result_out);
        return overlay_finish(boundary, result_out, 0u, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result_out->cpu_delta, 0u,
                (uint16_t)value) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result_out->cpu_delta, 2u,
                (uint16_t)(value >> 16));
    }
    if (service == 0x02u) {
        value = ((cpu->ecx & 0xffffu) << 16) | (cpu->edx & 0xffffu);
        if ((value != UINT32_MAX && !bx_ntvdm_dem_overlay_handle_backend_v1_seek(
                &provider->files, &provider->overlay_files, token(cpu), (int32_t)value,
                BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_BEGIN, &count)) ||
            !bx_ntvdm_dem_overlay_handle_backend_v1_close(&provider->files,
                &provider->overlay_files, token(cpu))) return overlay_error(boundary, result_out);
        return overlay_finish(boundary, result_out, 0u, 0);
    }
    if (service == 0x27u) return bx_ntvdm_dem_overlay_handle_backend_v1_flush(
        &provider->files, token(cpu)) ? overlay_finish(boundary, result_out, 0u, 0) :
        overlay_error(boundary, result_out);
    if (service == 0x1eu && count == 0u)
        return bx_ntvdm_dem_overlay_handle_backend_v1_truncate(&provider->files,
            &provider->overlay_files, token(cpu)) ? overlay_finish(boundary, result_out, 0u, 0) :
            overlay_error(boundary, result_out);
    if (service != 0x16u || count == 0u || !overlay_seek_if_requested(provider, cpu) ||
        !physical(cpu->ds, (uint16_t)cpu->edx, count, &address) ||
        !bx_ntvdm_dem_overlay_handle_backend_v1_read(&provider->files,
            &provider->overlay_files, token(cpu), bytes, count, &value))
        return overlay_error(boundary, result_out);
    if (!overlay_finish(boundary, result_out, 0u, 0) ||
        !bx_ntvdm_cpu_delta_v1_set_gpr16(&result_out->cpu_delta, 0u,
            (uint16_t)value)) return 0;
    return value == 0u || write_action(provider, address, bytes, value, action_out);
}

int bx_ntvdm_dem_handle_route_partition_v1_owns_service(uint8_t service)
{
    return service == 0x00u || service == 0x02u || service == 0x08u ||
        service == 0x16u || service == 0x1eu || service == 0x27u;
}

int bx_ntvdm_dem_handle_route_partition_v1_claims_request(
    const bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_cpu_state_v1 *cpu)
{
    HANDLE unused;
    uint32_t backend_token;
    if (provider == 0 || cpu == 0 ||
        !bx_ntvdm_dem_handle_route_partition_v1_owns_service(service)) return 0;
    if (service == 0x08u && (uint8_t)(cpu->ebx & 0xffu) > 1u) return 1;
    return readonly_token(provider, cpu, &backend_token) || overlay_token(provider, cpu) ||
        (service == 0x02u && token(cpu) == 0u) ||
        bx_ntvdm_dem_file_session_v1_lookup(&provider->files, token(cpu), &unused);
}

int bx_ntvdm_dem_handle_route_partition_v1_dispatch(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    struct bx_ntvdm_mechanical_action_v1 *action_out,
    bx_ntvdm_cpu_result_v2 *result_out)
{
    uint32_t count, used = 0u;
    uint64_t address;
    bx_ntvdm_guest_range range;
    bx_ntvdm_guest_gather_read_action_v1 gather;
    uint8_t bytes[BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES];
    if (action_out == 0 || result_out == 0 || !matches(service, boundary, cpu, window) ||
        !bx_ntvdm_dem_handle_route_partition_v1_owns_service(service) ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider)) return 0;
    count = (uint16_t)cpu->ecx;
    bx_ntvdm_mechanical_action_v1_clear(action_out);
    bx_ntvdm_cpu_result_v2_pass_through(result_out);
    if (startup_overlay_dispatch(provider, service, boundary, cpu, window,
            action_out, result_out)) return bx_ntvdm_cpu_result_v2_valid(result_out);
    if (overlay_token(provider, cpu)) {
        if (service == 0x1eu && count != 0u) {
            if (!physical(cpu->ds, (uint16_t)cpu->edx, count, &address)) return 0;
            range.address = address;
            range.length = count;
            return bx_ntvdm_dem_whole_provider_v1_prepare_gather(provider, service,
                boundary, cpu, &range, 1u, &gather) && read_action(provider, &gather, action_out);
        }
        return overlay_handle_dispatch(provider, service, boundary, cpu, action_out, result_out) &&
            bx_ntvdm_cpu_result_v2_valid(result_out);
    }
    if (service == 0x1eu && count != 0u) {
        if (!physical(cpu->ds, (uint16_t)cpu->edx, count, &address)) return 0;
        range.address = address;
        range.length = count;
        return bx_ntvdm_dem_whole_provider_v1_prepare_gather(provider, service,
            boundary, cpu, &range, 1u, &gather) && read_action(provider, &gather, action_out);
    }
    if (service == 0x16u && count != 0u) {
        if (!physical(cpu->ds, (uint16_t)cpu->edx, count, &address) ||
            !bx_ntvdm_dem_handle_partition_v1_dispatch(provider, service, boundary,
                cpu, bytes, count, &used, result_out) || used > count) return 0;
        return used == 0u || write_action(provider, address, bytes, used, action_out);
    }
    return bx_ntvdm_dem_handle_partition_v1_dispatch(provider, service, boundary,
        cpu, 0, 0u, &used, result_out) && bx_ntvdm_cpu_result_v2_valid(result_out);
}

int bx_ntvdm_dem_handle_route_partition_v1_complete_read(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const struct bx_ntvdm_mechanical_action_v1 *read_action_in,
    bx_ntvdm_cpu_result_v2 *result_out)
{
    bx_ntvdm_guest_gather_read_action_v1 gather;
    uint8_t copied[BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES];
    uint32_t copied_count = 0u, used = 0u;
    if (result_out == 0 || boundary == 0 || cpu == 0 || read_action_in == 0 || service != 0x1eu ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider) ||
        read_action_in->kind != BX_NTVDM_MECHANICAL_ACTION_V1_READ ||
        !bx_ntvdm_mechanical_action_v1_valid(read_action_in)) return 0;
    gather = provider->pending_gather;
    if (!bx_ntvdm_dem_whole_provider_v1_complete_gather(provider, service,
        boundary, cpu, &gather, read_action_in->payload, read_action_in->payload_bytes,
        copied, &copied_count) || copied_count != (uint16_t)cpu->ecx) return 0;
    if (overlay_token(provider, cpu)) {
        return bx_ntvdm_dem_overlay_handle_backend_v1_write(&provider->files,
            &provider->overlay_files, token(cpu), copied, copied_count, &used) &&
            overlay_finish(boundary, result_out, 0u, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result_out->cpu_delta, 0u,
                (uint16_t)used) &&
            bx_ntvdm_cpu_result_v2_valid(result_out);
    }
    return bx_ntvdm_dem_handle_partition_v1_dispatch(provider, service, boundary,
        cpu, copied, copied_count, &used, result_out) && bx_ntvdm_cpu_result_v2_valid(result_out);
}
