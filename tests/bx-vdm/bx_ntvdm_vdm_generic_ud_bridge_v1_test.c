#include "bop-v1/bx_ntvdm_vdm_generic_ud_bridge_v1.h"

#include <string.h>

static void initialize(struct bx_ntvdm_generic_ud_event_v1 *event,
    const uint8_t *bytes, uint32_t byte_count)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = (uint32_t)sizeof(*event);
    event->vector = 6u;
    event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event->fault_rip = 0x400u;
    event->eip = 0x400u;
    event->cs = 0x70u;
    event->window_bytes = byte_count;
    memcpy(event->window, bytes, byte_count);
}

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    uint8_t dem[] = {0xc4u, 0xc4u, 0x50u, 0x11u};
    uint8_t not_bop[] = {0x90u};
    uint8_t incomplete[] = {0xc4u, 0xc4u, 0x50u};
    uint8_t unknown[] = {0xc4u, 0xc4u, 0x58u};
    int failed = 0;

    initialize(&event, dem, sizeof(dem));
    failed |= !bx_ntvdm_vdm_generic_ud_classify_v1(&event, &ingress, &selection) ||
        ingress.route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
        ingress.family != BX_NTVDM_BOP_FAMILY_DEM || ingress.service != 0x11u ||
        selection.disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED ||
        selection.provider_family != BX_NTVDM_BOP_PROVIDER_DEM;
    memset(&outcome, 0, sizeof(outcome));
    failed |= bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) != 0;
    initialize(&event, not_bop, sizeof(not_bop));
    failed |= !bx_ntvdm_vdm_generic_ud_classify_v1(&event, &ingress, &selection) ||
        ingress.route != BX_NTVDM_BOP_ROUTE_NOT_BOP ||
        selection.disposition != BX_NTVDM_BOP_PROVIDER_NOT_APPLICABLE;
    initialize(&event, incomplete, sizeof(incomplete));
    failed |= !bx_ntvdm_vdm_generic_ud_classify_v1(&event, &ingress, &selection) ||
        ingress.route != BX_NTVDM_BOP_ROUTE_INCOMPLETE;
    initialize(&event, unknown, sizeof(unknown));
    failed |= !bx_ntvdm_vdm_generic_ud_classify_v1(&event, &ingress, &selection) ||
        ingress.route != BX_NTVDM_BOP_ROUTE_UNKNOWN_SELECTOR;
    return failed ? 1 : 0;
}
