#include "adapter-bop/bx_ntvdm_bop_ingress_v1.h"

#include <string.h>

static int calls;

static int route(const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome, void *context)
{
    if (event == 0 || outcome == 0 || context != &calls ||
        event->window_bytes != 3u || event->window[0] != 0xc4u) return 0;
    ++calls;
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = BX_NTVDM_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 3u;
    return 1;
}

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event.struct_bytes = sizeof(event); event.vector = 6u;
    event.fault_rip = 0x1000u; event.window_bytes = 3u;
    event.window[0] = 0xc4u; event.window[1] = 0xc4u; event.window[2] = 0x50u;
    if (bx_ntvdm_bop_ingress_v1_bound() ||
        bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 1;
    if (!bx_ntvdm_bop_ingress_v1_bind(route, &calls) ||
        bx_ntvdm_bop_ingress_v1_bind(route, &calls) ||
        !bx_ntvdm_bop_ingress_v1_bound()) return 2;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) || calls != 1 ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x1003u) return 3;
    bx_ntvdm_bop_ingress_v1_unbind();
    return !bx_ntvdm_bop_ingress_v1_bound() &&
        !bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ? 0 : 4;
}
