#include "bx_ntvdm_dpmi_plane_v1.h"
#include <string.h>

void bx_ntvdm_dpmi_plane_v1_clear(bx_ntvdm_dpmi_plane_record_v1 *record)
{
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->magic = BX_NTVDM_DPMI_PLANE_V1_MAGIC;
    record->abi_version = BX_NTVDM_DPMI_PLANE_V1_VERSION;
    record->struct_bytes = (uint32_t)sizeof(*record);
}

int bx_ntvdm_dpmi_plane_v1_valid(const bx_ntvdm_dpmi_plane_record_v1 *record)
{
    return record != NULL && record->magic == BX_NTVDM_DPMI_PLANE_V1_MAGIC &&
        record->abi_version == BX_NTVDM_DPMI_PLANE_V1_VERSION &&
        record->struct_bytes == sizeof(*record) &&
        record->component > BX_NTVDM_DPMI_COMPONENT_NONE &&
        record->component <= BX_NTVDM_DPMI_COMPONENT_VDD &&
        record->machine_owner > BX_NTVDM_DPMI_MACHINE_OWNER_NONE &&
        record->machine_owner <= BX_NTVDM_DPMI_MACHINE_OWNER_COMPOSITION &&
        record->disposition == BX_NTVDM_BOP_PROVIDER_EXPLICIT_UNAVAILABLE;
}

static uint32_t component(uint8_t service)
{
    switch (service) {
    case 0: case 1: case 3: case 4: case 5: case 15: case 17:
        return BX_NTVDM_DPMI_COMPONENT_DESCRIPTOR_MODE;
    case 2: case 10: case 20: case 21: case 22: case 23: case 24:
        return BX_NTVDM_DPMI_COMPONENT_INTERRUPT_FAULT;
    case 6: return BX_NTVDM_DPMI_COMPONENT_DOS_BRIDGE;
    case 7: case 8: case 9: case 11: case 16: case 19:
        return BX_NTVDM_DPMI_COMPONENT_EXTENDED_MEMORY;
    case 12: case 13: return BX_NTVDM_DPMI_COMPONENT_SESSION;
    case 14: return BX_NTVDM_DPMI_COMPONENT_DEBUG;
    case 18: return BX_NTVDM_DPMI_COMPONENT_VDD;
    default: return BX_NTVDM_DPMI_COMPONENT_NONE;
    }
}

static uint32_t owner(uint32_t value)
{
    switch (value) {
    case BX_NTVDM_DPMI_COMPONENT_EXTENDED_MEMORY:
        return BX_NTVDM_DPMI_MACHINE_OWNER_BOCHS_MEMORY;
    case BX_NTVDM_DPMI_COMPONENT_DESCRIPTOR_MODE:
    case BX_NTVDM_DPMI_COMPONENT_DEBUG:
        return BX_NTVDM_DPMI_MACHINE_OWNER_BOCHS_CPU;
    case BX_NTVDM_DPMI_COMPONENT_INTERRUPT_FAULT:
        return BX_NTVDM_DPMI_MACHINE_OWNER_BOCHS_INTERRUPT;
    default: return BX_NTVDM_DPMI_MACHINE_OWNER_COMPOSITION;
    }
}

int bx_ntvdm_dpmi_plane_v1_classify(const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    bx_ntvdm_dpmi_plane_record_v1 *record)
{
    uint32_t value;
    if (record == NULL || !bx_ntvdm_bop_ingress_v1_valid(ingress) ||
        !bx_ntvdm_bop_provider_selection_v1_valid(selection)) return 0;
    bx_ntvdm_dpmi_plane_v1_clear(record);
    if (ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
        ingress->family != BX_NTVDM_BOP_FAMILY_DPMI || ingress->service >= 25u ||
        selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED ||
        selection->precedence != BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT ||
        selection->provider_family != BX_NTVDM_BOP_PROVIDER_DPMI ||
        (value = component(ingress->service)) == BX_NTVDM_DPMI_COMPONENT_NONE) return 0;
    record->service = ingress->service;
    record->component = value;
    record->machine_owner = owner(value);
    record->disposition = BX_NTVDM_BOP_PROVIDER_EXPLICIT_UNAVAILABLE;
    return bx_ntvdm_dpmi_plane_v1_valid(record);
}
