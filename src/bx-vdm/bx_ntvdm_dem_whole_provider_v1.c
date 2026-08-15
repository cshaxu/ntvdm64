#include "bx_ntvdm_dem_whole_provider_v1.h"

#include <string.h>

int bx_ntvdm_dem_whole_provider_v1_owns_service(uint8_t service)
{
    switch (service) {
    case 0x00u: case 0x01u: case 0x02u: case 0x03u: case 0x04u:
    case 0x05u: case 0x06u: case 0x07u: case 0x08u: case 0x09u:
    case 0x0au: case 0x0bu: case 0x0cu: case 0x12u: case 0x16u:
    case 0x17u: case 0x1eu: case 0x20u: case 0x22u: case 0x27u:
    case 0x2cu: case 0x2du: case 0x2eu: case 0x2fu: case 0x30u:
    case 0x31u: case 0x44u: case 0x47u: case 0x48u:
        return 1;
    default:
        return 0;
    }
}

int bx_ntvdm_dem_whole_provider_v1_valid(
    const bx_ntvdm_dem_whole_provider_v1 *provider)
{
    return provider != 0 &&
        provider->magic == BX_NTVDM_DEM_WHOLE_PROVIDER_V1_MAGIC &&
        provider->abi_version == BX_NTVDM_DEM_WHOLE_PROVIDER_V1_VERSION &&
        provider->struct_bytes == sizeof(*provider) && provider->reserved0 == 0u &&
        bx_ntvdm_host_namespace_v1_valid(provider->host_namespace) &&
        bx_ntvdm_dem_cwd_context_v1_valid(provider->cwd) &&
        bx_ntvdm_dem_file_session_v1_valid(&provider->files) &&
        bx_ntvdm_dem_local_file_backend_v1_valid(&provider->local_files) &&
        bx_ntvdm_search_transaction_v1_valid(&provider->search) &&
        provider->path_search_dta_address < UINT64_C(0x100000) &&
        provider->next_action_id != 0u && provider->pending_service <= 0x48u &&
        ((provider->pending_action_id == 0u && provider->pending_bytes == 0u) ||
         (provider->pending_action_id != 0u &&
          bx_ntvdm_dem_whole_provider_v1_owns_service((uint8_t)provider->pending_service) &&
          bx_ntvdm_exception_event_v1_valid(&provider->pending_boundary) &&
          bx_ntvdm_cpu_state_v1_valid(&provider->pending_cpu) &&
          bx_ntvdm_guest_gather_read_action_v1_valid(&provider->pending_gather) &&
          provider->pending_gather.disposition == BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ &&
          provider->pending_bytes == provider->pending_gather.total_bytes));
}

int bx_ntvdm_dem_whole_provider_v1_initialize(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_mutation_profile_v1 *profile,
    const bx_ntvdm_host_namespace_v1 *host_namespace,
    const bx_ntvdm_dem_cwd_context_v1 *cwd)
{
    if (provider == 0 || profile == 0 ||
        !bx_ntvdm_mutation_profile_v1_valid(profile) ||
        !bx_ntvdm_host_namespace_v1_valid(host_namespace) ||
        !bx_ntvdm_dem_cwd_context_v1_valid(cwd)) return 0;
    memset(provider, 0, sizeof(*provider));
    provider->magic = BX_NTVDM_DEM_WHOLE_PROVIDER_V1_MAGIC;
    provider->abi_version = BX_NTVDM_DEM_WHOLE_PROVIDER_V1_VERSION;
    provider->struct_bytes = sizeof(*provider);
    provider->next_action_id = 1u;
    provider->host_namespace = host_namespace;
    provider->cwd = cwd;
    if (!bx_ntvdm_dem_file_session_v1_initialize(&provider->files, profile) ||
        !bx_ntvdm_dem_local_file_backend_v1_initialize(&provider->local_files,
            &provider->files, host_namespace, cwd)) {
        bx_ntvdm_dem_whole_provider_v1_teardown(provider);
        return 0;
    }
    bx_ntvdm_search_transaction_v1_initialize(&provider->search);
    if (!bx_ntvdm_search_transaction_v1_valid(&provider->search)) {
        bx_ntvdm_dem_whole_provider_v1_teardown(provider);
        return 0;
    }
    return bx_ntvdm_dem_whole_provider_v1_valid(provider);
}

void bx_ntvdm_dem_whole_provider_v1_teardown(
    bx_ntvdm_dem_whole_provider_v1 *provider)
{
    if (provider == 0) return;
    bx_ntvdm_search_transaction_v1_release(&provider->search);
    bx_ntvdm_dem_file_session_v1_teardown(&provider->files);
    memset(provider, 0, sizeof(*provider));
}

static void clear_pending(bx_ntvdm_dem_whole_provider_v1 *provider)
{
    provider->pending_service = 0u;
    provider->pending_action_id = 0u;
    provider->pending_bytes = 0u;
    memset(&provider->pending_boundary, 0, sizeof(provider->pending_boundary));
    memset(&provider->pending_cpu, 0, sizeof(provider->pending_cpu));
    memset(&provider->pending_gather, 0, sizeof(provider->pending_gather));
}

int bx_ntvdm_dem_whole_provider_v1_prepare_gather(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_range *ranges, uint32_t range_count,
    bx_ntvdm_guest_gather_read_action_v1 *action_out)
{
    bx_ntvdm_guest_gather_read_action_v1 action;
    if (!bx_ntvdm_dem_whole_provider_v1_valid(provider) || action_out == 0 ||
        !bx_ntvdm_dem_whole_provider_v1_owns_service(service) ||
        boundary == 0 || cpu_before == 0 || ranges == 0 || range_count == 0u ||
        provider->pending_action_id != 0u || !bx_ntvdm_exception_event_v1_valid(boundary) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) || boundary->fault_rip > UINT64_MAX - 4u ||
        !bx_ntvdm_guest_gather_read_action_v1_need_read_resume(&action, ranges,
            range_count, boundary->fault_rip + 4u)) return 0;
    provider->pending_service = service;
    provider->pending_action_id = provider->next_action_id++;
    if (provider->next_action_id == 0u) provider->next_action_id = 1u;
    provider->pending_bytes = action.total_bytes;
    provider->pending_boundary = *boundary;
    provider->pending_cpu = *cpu_before;
    provider->pending_gather = action;
    if (!bx_ntvdm_dem_whole_provider_v1_valid(provider)) {
        clear_pending(provider);
        return 0;
    }
    *action_out = action;
    return 1;
}

int bx_ntvdm_dem_whole_provider_v1_complete_gather(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint32_t byte_count,
    uint8_t copied_bytes[BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES],
    uint32_t *copied_byte_count)
{
    int accepted;
    if (copied_byte_count != 0) *copied_byte_count = 0u;
    accepted = provider != 0 && boundary != 0 && cpu_before != 0 && action != 0 &&
        bytes != 0 && copied_bytes != 0 && copied_byte_count != 0 &&
        bx_ntvdm_dem_whole_provider_v1_valid(provider) &&
        provider->pending_action_id != 0u && provider->pending_service == service &&
        memcmp(&provider->pending_boundary, boundary, sizeof(*boundary)) == 0 &&
        memcmp(&provider->pending_cpu, cpu_before, sizeof(*cpu_before)) == 0 &&
        memcmp(&provider->pending_gather, action, sizeof(*action)) == 0 &&
        byte_count == provider->pending_bytes;
    if (accepted) {
        memcpy(copied_bytes, bytes, byte_count);
        *copied_byte_count = byte_count;
    }
    if (provider != 0 && provider->pending_action_id != 0u) clear_pending(provider);
    return accepted;
}

int bx_ntvdm_dem_whole_provider_v1_cancel_gather(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action)
{
    if (!bx_ntvdm_dem_whole_provider_v1_valid(provider) || boundary == 0 ||
        cpu_before == 0 || action == 0 || provider->pending_action_id == 0u ||
        provider->pending_service != service || memcmp(&provider->pending_boundary,
            boundary, sizeof(*boundary)) != 0 || memcmp(&provider->pending_cpu,
            cpu_before, sizeof(*cpu_before)) != 0 || memcmp(&provider->pending_gather,
            action, sizeof(*action)) != 0) return 0;
    clear_pending(provider);
    return 1;
}
