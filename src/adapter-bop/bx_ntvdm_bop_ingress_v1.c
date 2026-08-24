#include "bx_ntvdm_bop_ingress_v1.h"

static bx_ntvdm_bop_route_v1 g_route;
static void *g_context;

static int event_valid(const struct bx_ntvdm_generic_ud_event_v1 *event)
{
    return event != 0 && event->magic == BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes <= BX_NTVDM_GENERIC_UD_WINDOW_BYTES;
}

int bx_ntvdm_bop_ingress_v1_bind(bx_ntvdm_bop_route_v1 route, void *context)
{
    if (route == 0 || g_route != 0) return 0;
    g_context = context;
    g_route = route;
    return 1;
}

void bx_ntvdm_bop_ingress_v1_unbind(void)
{
    g_route = 0;
    g_context = 0;
}

int bx_ntvdm_bop_ingress_v1_bound(void)
{
    return g_route != 0;
}

/* This is the adopted CPU's external mechanical symbol.  It contains no
 * selector/service recognition: an unbound composition simply declines. */
int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (!event_valid(event) || outcome == 0 || g_route == 0) return 0;
    return g_route(event, outcome, g_context) != 0;
}
