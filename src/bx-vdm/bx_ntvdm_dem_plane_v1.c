#include "bx_ntvdm_dem_plane_v1.h"

#include <string.h>

void bx_ntvdm_dem_plane_v1_clear(bx_ntvdm_dem_plane_record_v1 *record)
{
    if (record == 0) return;
    memset(record, 0, sizeof(*record));
    record->magic = BX_NTVDM_DEM_PLANE_V1_MAGIC;
    record->abi_version = BX_NTVDM_DEM_PLANE_V1_VERSION;
    record->struct_bytes = (uint32_t)sizeof(*record);
}

int bx_ntvdm_dem_plane_v1_valid(const bx_ntvdm_dem_plane_record_v1 *record)
{
    return record != 0 && record->magic == BX_NTVDM_DEM_PLANE_V1_MAGIC &&
        record->abi_version == BX_NTVDM_DEM_PLANE_V1_VERSION &&
        record->struct_bytes == sizeof(*record) && record->service < 73u &&
        record->component <= BX_NTVDM_DEM_COMPONENT_ORIGINAL_NOOP &&
        record->disposition <= BX_NTVDM_DEM_PLANE_ORIGINAL_NOOP &&
        record->reserved0 == 0u;
}

static uint32_t bx_ntvdm_dem_plane_v1_component(uint8_t service)
{
    switch (service) {
    case 0u: case 1u: case 2u: case 3u: case 4u: case 5u: case 6u:
    case 8u: case 9u: case 10u: case 11u: case 12u: case 17u: case 18u: case 19u:
    case 22u: case 23u: case 24u: case 30u: case 34u: case 39u: case 60u: case 66u:
    case 68u: case 71u: case 72u:
        return BX_NTVDM_DEM_COMPONENT_NAMESPACE;
    case 7u: case 32u: case 44u: case 45u: case 46u: case 47u: case 48u:
    case 49u:
        return BX_NTVDM_DEM_COMPONENT_FCB;
    case 13u: case 14u: case 15u: case 16u: case 20u: case 21u: case 25u:
    case 26u: case 27u: case 28u: case 37u: case 65u: case 70u:
        return BX_NTVDM_DEM_COMPONENT_GSET;
    case 33u: case 41u: case 42u:
        return BX_NTVDM_DEM_COMPONENT_RAW_MEDIA;
    case 35u: case 52u: case 53u: case 54u: case 55u: case 56u:
    case 57u: case 58u: case 59u: case 61u: case 62u: case 69u:
        return BX_NTVDM_DEM_COMPONENT_MISC;
    case 50u: case 51u: case 63u:
        return BX_NTVDM_DEM_COMPONENT_ERROR_LOCK;
    case 29u:
        return BX_NTVDM_DEM_COMPONENT_V86_BRIDGE;
    case 31u: case 36u: case 38u: case 40u: case 43u: case 64u:
    case 67u:
        return BX_NTVDM_DEM_COMPONENT_ORIGINAL_NOOP;
    default:
        return BX_NTVDM_DEM_COMPONENT_NONE;
    }
}

int bx_ntvdm_dem_plane_v1_classify(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    bx_ntvdm_dem_plane_record_v1 *record)
{
    uint32_t component;
    if (record == 0 || !bx_ntvdm_bop_ingress_v1_valid(ingress) ||
        !bx_ntvdm_bop_provider_selection_v1_valid(selection)) return 0;
    bx_ntvdm_dem_plane_v1_clear(record);
    if (ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
        ingress->family != BX_NTVDM_BOP_FAMILY_DEM ||
        selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED ||
        selection->provider_family != BX_NTVDM_BOP_PROVIDER_DEM ||
        (selection->precedence != BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT &&
         !(ingress->service == 60u && selection->precedence ==
            BX_NTVDM_BOP_PROVIDER_PRECEDENCE_SOURCE_DERIVED_AFTER_BLOCKER)) ||
        ingress->service >= 73u ||
        (component = bx_ntvdm_dem_plane_v1_component(ingress->service)) ==
            BX_NTVDM_DEM_COMPONENT_NONE)
        return 0;
    record->service = ingress->service;
    record->component = component;
    record->disposition = component == BX_NTVDM_DEM_COMPONENT_ORIGINAL_NOOP ?
        BX_NTVDM_DEM_PLANE_ORIGINAL_NOOP : BX_NTVDM_DEM_PLANE_DEFERRED;
    return bx_ntvdm_dem_plane_v1_valid(record);
}
