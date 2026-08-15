#include "bx_ntvdm_dem_search_partition_v1.h"

#include "bx_ntvdm_dem_fcb_search_service_v1.h"
#include "bx_ntvdm_dem_path_search_service_v1.h"

#include <string.h>

#define BX_NTVDM_DEM_SEARCH_APERTURE UINT64_C(0x100000)

static uint16_t word(const uint8_t *value)
{ return (uint16_t)value[0] | ((uint16_t)value[1] << 8); }

static int real_address(uint16_t segment, uint16_t offset, uint64_t bytes,
    uint64_t *address_out)
{
    uint64_t address = ((uint64_t)segment << 4) + offset;
    if (address_out == 0 || bytes == 0u ||
        address > BX_NTVDM_DEM_SEARCH_APERTURE - bytes) return 0;
    *address_out = address;
    return 1;
}

int bx_ntvdm_dem_search_partition_v1_owns_service(uint8_t service)
{
    return service == 0x09u || service == 0x0au || service == 0x0bu ||
        service == 0x0cu;
}

int bx_ntvdm_dem_search_partition_v1_prepare(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_dem_dta_registration_v1 *registration, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_gather_read_action_v1 *action_out)
{
    bx_ntvdm_guest_gather_read_action_v1 gathered;
    int prepared;
    if (!bx_ntvdm_dem_whole_provider_v1_valid(provider) || registration == 0 ||
        boundary == 0 || cpu_before == 0 || window == 0 || action_out == 0)
        return 0;
    switch (service) {
    case 0x09u:
        prepared = bx_ntvdm_dem_path_search_v1_prepare_first(registration,
            boundary, cpu_before, window, &gathered);
        break;
    case 0x0au:
        prepared = bx_ntvdm_dem_fcb_search_service_v1_prepare_first(registration,
            boundary, cpu_before, window, &gathered);
        break;
    case 0x0bu:
        prepared = bx_ntvdm_dem_path_search_v1_prepare_next(registration,
            boundary, cpu_before, window, &gathered);
        /* PATHNEXT consumes the actual DTA resolved by PATHFIRST, not the
         * four-byte far-pointer variable that supplied it. */
        if (prepared && provider->path_search_dta_address != 0u)
            gathered.ranges[0].address = provider->path_search_dta_address;
        else prepared = 0;
        break;
    case 0x0cu:
        prepared = bx_ntvdm_dem_fcb_search_service_v1_prepare_next(registration,
            boundary, cpu_before, window, &gathered);
        break;
    default:
        return 0;
    }
    return prepared && bx_ntvdm_dem_whole_provider_v1_prepare_gather(provider,
        service, boundary, cpu_before, gathered.ranges, gathered.range_count,
        action_out);
}

int bx_ntvdm_dem_search_partition_v1_complete(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint32_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[51], uint32_t *payload_bytes)
{
    uint8_t copied[BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES];
    uint32_t copied_bytes = 0u;
    uint64_t output_address;
    int result;
    if (payload_bytes != 0) *payload_bytes = 0u;
    if (!bx_ntvdm_dem_search_partition_v1_owns_service(service) ||
        transaction == 0 || payload == 0 || payload_bytes == 0 ||
        !bx_ntvdm_dem_whole_provider_v1_complete_gather(provider, service,
            boundary, cpu_before, action, bytes, byte_count, copied,
            &copied_bytes)) return 0;
    switch (service) {
    case 0x09u:
        if (copied_bytes != 134u || action->range_count != 3u ||
            action->ranges[0].length != 128u || action->ranges[1].length != 4u ||
            action->ranges[2].length != 2u || !real_address(word(copied + 130u),
            word(copied + 128u), 43u, &output_address)) return 0;
        result = bx_ntvdm_search_transaction_v1_path_first_merged(&provider->search,
            provider->host_namespace, provider->declared_search_snapshot, boundary, cpu_before, word(copied + 132u),
            output_address, copied, (uint16_t)cpu_before->ecx, transaction,
            payload, payload_bytes);
        if (result >= 0) provider->path_search_dta_address = output_address;
        break;
    case 0x0au:
        if (copied_bytes != 183u || action->range_count != 3u ||
            action->ranges[0].length != 53u || action->ranges[1].length != 128u ||
            action->ranges[2].length != 2u) return 0;
        result = bx_ntvdm_search_transaction_v1_fcb_first_merged(&provider->search,
            provider->host_namespace, provider->declared_search_snapshot, boundary, cpu_before, word(copied + 181u),
            action->ranges[0].address, copied + 53u, (uint8_t)cpu_before->eax,
            (uint8_t)cpu_before->edx, transaction, payload, payload_bytes);
        break;
    case 0x0bu:
        if (copied_bytes != 45u || action->range_count != 2u ||
            action->ranges[0].length != 43u || action->ranges[1].length != 2u)
            return 0;
        result = bx_ntvdm_search_transaction_v1_path_next(&provider->search,
            boundary, cpu_before, word(copied + 43u), action->ranges[0].address,
            copied, transaction, payload, payload_bytes);
        if (result <= 0) provider->path_search_dta_address = 0u;
        break;
    default:
        if (copied_bytes != 55u || action->range_count != 2u ||
            action->ranges[0].length != 53u || action->ranges[1].length != 2u)
            return 0;
        result = bx_ntvdm_search_transaction_v1_fcb_next(&provider->search,
            boundary, cpu_before, word(copied + 53u), action->ranges[0].address,
            copied, transaction, payload, payload_bytes);
        break;
    }
    return result >= 0;
}
