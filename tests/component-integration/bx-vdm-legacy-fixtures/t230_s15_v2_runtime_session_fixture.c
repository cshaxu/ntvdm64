#include <stdio.h>
#include <string.h>

#include "opennt-bop/ingress/dem_v2_runtime_session.h"
#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"
#include "opennt-bop/ingress/opennt_bop_route.h"

static void event_initialize(struct bx_ntvdm_generic_ud_event_v1 *event,
    uint8_t selector, uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = 1u;
    event->fault_rip = 0x2600u;
    event->eip = 0x2600u;
    event->window_bytes = 4u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = selector;
    event->window[3] = service;
}

int main(void)
{
    if (!bx_ntvdm_bop_ingress_v1_bind(bx_ntvdm_opennt_bop_route_dispatch_v1, 0)) return 90;
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;

    if (!bx_ntvdm_dem_v2_runtime_session_bind()) return 1;
    event_initialize(&event, 0x50u, 0x1fu);
    memset(&outcome, 0, sizeof(outcome));
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x2604u) return 2;
    event_initialize(&event, 0x54u, 0x1fu);
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 3;
    bx_ntvdm_dem_v2_runtime_session_reset();
    event_initialize(&event, 0x50u, 0x1fu);
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 4;
    puts("T230 v2 runtime session owns DEM and declines without legacy fallback");
    return 0;
}