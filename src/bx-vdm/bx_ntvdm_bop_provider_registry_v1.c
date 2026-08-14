#include "bx_ntvdm_bop_provider_registry_v1.h"

#include <string.h>

void bx_ntvdm_bop_provider_selection_v1_clear(
    bx_ntvdm_bop_provider_selection_v1 *selection)
{
    if (selection == 0) return;
    memset(selection, 0, sizeof(*selection));
    selection->magic = BX_NTVDM_BOP_PROVIDER_SELECTION_V1_MAGIC;
    selection->abi_version = BX_NTVDM_BOP_PROVIDER_SELECTION_V1_VERSION;
    selection->struct_bytes = (uint32_t)sizeof(*selection);
}

int bx_ntvdm_bop_provider_selection_v1_valid(
    const bx_ntvdm_bop_provider_selection_v1 *selection)
{
    return selection != 0 &&
        selection->magic == BX_NTVDM_BOP_PROVIDER_SELECTION_V1_MAGIC &&
        selection->abi_version == BX_NTVDM_BOP_PROVIDER_SELECTION_V1_VERSION &&
        selection->struct_bytes == sizeof(*selection) &&
        selection->disposition <= BX_NTVDM_BOP_PROVIDER_EXPLICIT_UNAVAILABLE &&
        selection->provider_family <= BX_NTVDM_BOP_PROVIDER_TOP_LEVEL &&
        selection->precedence <=
            BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_FAILURE_OR_DEFERRED &&
        selection->reserved0 == 0u;
}

static uint32_t bx_ntvdm_bop_provider_registry_v1_family(
    uint16_t ingress_family)
{
    switch (ingress_family) {
    case BX_NTVDM_BOP_FAMILY_DEM: return BX_NTVDM_BOP_PROVIDER_DEM;
    case BX_NTVDM_BOP_FAMILY_WOW: return BX_NTVDM_BOP_PROVIDER_WOW;
    case BX_NTVDM_BOP_FAMILY_XMS: return BX_NTVDM_BOP_PROVIDER_XMS;
    case BX_NTVDM_BOP_FAMILY_DPMI: return BX_NTVDM_BOP_PROVIDER_DPMI;
    case BX_NTVDM_BOP_FAMILY_COMMAND: return BX_NTVDM_BOP_PROVIDER_COMMAND;
    case BX_NTVDM_BOP_FAMILY_DEBUGGER: return BX_NTVDM_BOP_PROVIDER_DEBUGGER;
    case BX_NTVDM_BOP_FAMILY_REDIR: return BX_NTVDM_BOP_PROVIDER_REDIR;
    case BX_NTVDM_BOP_FAMILY_TOP_LEVEL: return BX_NTVDM_BOP_PROVIDER_TOP_LEVEL;
    default: return BX_NTVDM_BOP_PROVIDER_NONE;
    }
}

int bx_ntvdm_bop_provider_registry_v1_select(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    bx_ntvdm_bop_provider_selection_v1 *selection)
{
    if (selection == 0 || !bx_ntvdm_bop_ingress_v1_valid(ingress)) return 0;
    bx_ntvdm_bop_provider_selection_v1_clear(selection);
    if (ingress->route == BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED) {
        selection->disposition = BX_NTVDM_BOP_PROVIDER_DEFERRED;
        selection->provider_family =
            bx_ntvdm_bop_provider_registry_v1_family(ingress->family);
        selection->precedence = ingress->family == BX_NTVDM_BOP_FAMILY_DEM &&
            ingress->has_service != 0u && ingress->service == 66u ?
            BX_NTVDM_BOP_PROVIDER_PRECEDENCE_SOURCE_DERIVED_AFTER_BLOCKER :
            BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT;
    } else if (ingress->route == BX_NTVDM_BOP_ROUTE_EXPLICIT_UNAVAILABLE) {
        selection->disposition = BX_NTVDM_BOP_PROVIDER_EXPLICIT_UNAVAILABLE;
        selection->provider_family = BX_NTVDM_BOP_PROVIDER_TOP_LEVEL;
        selection->precedence =
            BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_FAILURE_OR_DEFERRED;
    }
    return bx_ntvdm_bop_provider_selection_v1_valid(selection);
}
