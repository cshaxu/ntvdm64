#include "bx_ntvdm_dem_drive_view_provider_v1.h"
#include "bx_ntvdm_dem_computer_name_service_v1.h"
#include "bx_ntvdm_dem_cwd_service_v2.h"
#include "bx_ntvdm_dem_dta_service.h"
#include "bx_ntvdm_dem_dpb_service.h"
#include "bx_ntvdm_dem_full_dpb_service_v1.h"
#include "bx_ntvdm_dem_media_id_service_v1.h"

#include <string.h>

void bx_ntvdm_dem_drive_view_provider_v1_clear(
    bx_ntvdm_dem_drive_view_provider_v1 *provider)
{
    if (provider == 0) return;
    memset(provider, 0, sizeof(*provider));
}

int bx_ntvdm_dem_drive_view_provider_v1_valid(
    const bx_ntvdm_dem_drive_view_provider_v1 *provider)
{
    return provider != 0 &&
        provider->magic == BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_MAGIC &&
        provider->abi_version == BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_VERSION &&
        provider->struct_bytes == sizeof(*provider) && provider->initialized == 1u &&
        provider->has_mutation_profile <= 1u && provider->has_boot_drive <= 1u &&
        (!provider->gset.has_drive_snapshot ||
            bx_ntvdm_host_drive_snapshot_v1_valid(&provider->gset.drive_snapshot)) &&
        (!provider->gset.has_volume_snapshot ||
            bx_ntvdm_host_volume_snapshot_v1_valid(&provider->gset.volume_snapshot)) &&
        (!provider->has_boot_drive || (provider->boot_drive_index < 26u &&
            provider->gset.has_drive_snapshot &&
            (provider->gset.drive_snapshot.admitted_mask &
                (UINT32_C(1) << provider->boot_drive_index)) != 0u)) &&
        (!provider->has_mutation_profile ||
            (bx_ntvdm_dem_profile_consumer_v1_valid(&provider->mutation_profile) &&
             bx_ntvdm_dem_cwd_context_v1_valid(&provider->cwd)));
}

int bx_ntvdm_dem_drive_view_provider_v1_initialize(
    bx_ntvdm_dem_drive_view_provider_v1 *provider)
{
    if (provider == 0) return 0;
    bx_ntvdm_dem_drive_view_provider_v1_clear(provider);
    provider->magic = BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_MAGIC;
    provider->abi_version = BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_VERSION;
    provider->struct_bytes = (uint32_t)sizeof(*provider);
    bx_ntvdm_dem_gset_plane_v1_clear(&provider->gset);
    provider->initialized = 1u;
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider);
}

int bx_ntvdm_dem_drive_view_provider_v1_set_drive_snapshot(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot)
{
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider) &&
        bx_ntvdm_dem_gset_plane_v1_set_drive_snapshot(&provider->gset, snapshot) &&
        bx_ntvdm_dem_drive_view_provider_v1_valid(provider);
}

int bx_ntvdm_dem_drive_view_provider_v1_set_volume_snapshot(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_host_volume_snapshot_v1 *snapshot)
{
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider) &&
        bx_ntvdm_dem_gset_plane_v1_set_volume_snapshot(&provider->gset, snapshot) &&
        bx_ntvdm_dem_drive_view_provider_v1_valid(provider);
}

int bx_ntvdm_dem_drive_view_provider_v1_set_mutation_profile(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_mutation_profile_v1 *profile)
{
    if (!bx_ntvdm_dem_drive_view_provider_v1_valid(provider) || profile == 0 ||
        provider->has_mutation_profile ||
        !bx_ntvdm_dem_profile_consumer_v1_initialize(&provider->mutation_profile, profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&provider->cwd, profile)) return 0;
    provider->has_mutation_profile = 1u;
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider);
}

int bx_ntvdm_dem_drive_view_provider_v1_set_boot_drive(
    bx_ntvdm_dem_drive_view_provider_v1 *provider, uint32_t drive_index)
{
    if (!bx_ntvdm_dem_drive_view_provider_v1_valid(provider) ||
        provider->has_boot_drive || drive_index >= 26u ||
        !provider->gset.has_drive_snapshot ||
        (provider->gset.drive_snapshot.admitted_mask & (UINT32_C(1) << drive_index)) == 0u)
        return 0;
    provider->boot_drive_index = drive_index;
    provider->has_boot_drive = 1u;
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider);
}

int bx_ntvdm_dem_drive_view_provider_v1_dispatch_observation(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD],
    uint32_t *needs_write,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (!bx_ntvdm_dem_drive_view_provider_v1_valid(provider) ||
        !ingress || !selection || !event || !cpu_before || !window ||
        !transaction || !payload || !needs_write || !result ||
        !bx_ntvdm_dem_drive_view_provider_v1_owns_observation(ingress->service))
        return 0;
    *needs_write = 0u;
    if (bx_ntvdm_dem_gset_plane_v1_dispatch(&provider->gset, ingress,
            selection, event, cpu_before, window, result))
        return 1;
    if (provider->gset.has_volume_snapshot &&
        bx_ntvdm_dem_media_id_service_v1_prepare(&provider->gset.volume_snapshot,
            event, cpu_before, window, transaction, payload)) {
        *needs_write = transaction->writes.write_count != 0u;
        *result = transaction->result;
        return 1;
    }
    if (provider->gset.has_volume_snapshot &&
        bx_ntvdm_dem_media_id_service_v1_snapshot_failure(
            &provider->gset.volume_snapshot, event, cpu_before, window, result))
        return 1;
    if (bx_ntvdm_dem_media_id_service_v1_set_failure(event, cpu_before,
            window, result))
        return 1;
    if (bx_ntvdm_dem_computer_name_service_v1_prepare(event, cpu_before,
            window, transaction, payload)) {
        *needs_write = transaction->writes.write_count != 0u;
        *result = transaction->result;
        return 1;
    }
    if (provider->gset.has_volume_snapshot &&
        bx_ntvdm_dem_full_dpb_service_v1_prepare(&provider->gset.volume_snapshot,
            event, cpu_before, window, transaction, payload)) {
        *needs_write = transaction->writes.write_count != 0u;
        *result = transaction->result;
        return 1;
    }
    if (provider->gset.has_volume_snapshot &&
        bx_ntvdm_dem_full_dpb_service_v1_snapshot_failure(
            &provider->gset.volume_snapshot, event, cpu_before, window, result))
        return 1;
    if (provider->gset.has_drive_snapshot &&
        bx_ntvdm_dem_dpb_service_v1_prepare(provider->gset.drive_snapshot.types,
            event, cpu_before, window, transaction, payload)) {
        *needs_write = transaction->writes.write_count != 0u;
        *result = transaction->result;
        return 1;
    }
    return 0;
}

int bx_ntvdm_dem_drive_view_provider_v1_prepare_cwd(
    const bx_ntvdm_dem_drive_view_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_guest_gather_read_action_v1 *action)
{
    uint32_t mode;
    if (!bx_ntvdm_dem_drive_view_provider_v1_valid(provider) ||
        !provider->has_mutation_profile ||
        !bx_ntvdm_dem_profile_consumer_v1_resolve(&provider->mutation_profile,
            BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, &mode) ||
        (mode != BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST &&
         mode != BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY))
        return 0;
    return bx_ntvdm_dem_cwd_service_v2_prepare(service, event, cpu, action);
}

int bx_ntvdm_dem_drive_view_provider_v1_complete_cwd(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t service,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{
    uint32_t mode;
    if (!bx_ntvdm_dem_drive_view_provider_v1_valid(provider) ||
        !provider->has_mutation_profile ||
        !bx_ntvdm_dem_profile_consumer_v1_resolve(&provider->mutation_profile,
            BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, &mode) ||
        (mode != BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST &&
         mode != BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY))
        return 0;
    return bx_ntvdm_dem_cwd_service_v2_complete(&provider->cwd, host_namespace,
        service, event, cpu, action, bytes, byte_count, transaction, payload);
}
int bx_ntvdm_dem_drive_view_provider_v1_owns_observation(uint8_t service)
{ return service == 0x0eu || service == 0x0fu || service == 0x10u ||
    service == 0x14u || service == 0x15u || service == 0x19u ||
    service == 0x1cu || service == 0x25u || service == 0x41u ||
    service == 0x46u; }

int bx_ntvdm_dem_drive_view_provider_v1_prepare_dta(
    const bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_read_action_v1 *action)
{
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider) &&
        provider->has_mutation_profile &&
        bx_ntvdm_dem_dta_service_v1_dispatch(event, cpu, window, action);
}

int bx_ntvdm_dem_drive_view_provider_v1_complete_dta(
    const bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_read_action_v1 *action, const uint8_t *bytes,
    uint64_t byte_count, bx_ntvdm_dem_dta_registration_v1 *registration,
    bx_ntvdm_cpu_result_v2 *result)
{
    return bx_ntvdm_dem_drive_view_provider_v1_valid(provider) &&
        provider->has_mutation_profile &&
        bx_ntvdm_dem_dta_service_v1_complete(event, cpu, action, bytes,
            byte_count, registration, result);
}