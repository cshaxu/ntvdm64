#include "bop_ingress.h"

static runtime_bop_route_v1 g_route;
static void *g_context;

static int event_valid(const struct runtime_generic_ud_event_v1 *event)
{
    return event != 0 && event->magic == RUNTIME_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes <= RUNTIME_GENERIC_UD_WINDOW_BYTES;
}

int runtime_bop_ingress_v1_bind(runtime_bop_route_v1 route, void *context)
{
    if (route == 0 || g_route != 0) return 0;
    g_context = context;
    g_route = route;
    return 1;
}

void runtime_bop_ingress_v1_unbind(void)
{
    g_route = 0;
    g_context = 0;
}

int runtime_bop_ingress_v1_bound(void)
{
    return g_route != 0;
}

/* This is the adopted CPU's external mechanical symbol.  It contains no
 * selector/service recognition: an unbound composition simply declines. */
int runtime_mantle_generic_ud_bridge_v1(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome)
{
    if (!event_valid(event) || outcome == 0 || g_route == 0) return 0;
    return g_route(event, outcome, g_context) != 0;
}


/* App registers this same-process wrapper with the selector-blind machine
 * callback slot.  The CPU side sees only opaque bytes; selector routing stays
 * entirely behind this adapter boundary. */
int runtime_bop_ingress_v1_opaque_callback(void *context,
    const void *event, unsigned event_bytes, void *outcome,
    unsigned outcome_bytes)
{
    (void)context;
    if (event == 0 || event_bytes != sizeof(struct runtime_generic_ud_event_v1) ||
        outcome == 0) return 0;
    if (outcome_bytes == sizeof(struct runtime_generic_ud_outcome_v2)) {
        return runtime_mantle_generic_ud_bridge_v2(
            (const struct runtime_generic_ud_event_v1 *)event,
            (struct runtime_generic_ud_outcome_v2 *)outcome);
    }
    if (outcome_bytes == sizeof(struct runtime_generic_ud_outcome_v1)) {
        int accepted = runtime_mantle_generic_ud_bridge_v1(
            (const struct runtime_generic_ud_event_v1 *)event,
            (struct runtime_generic_ud_outcome_v1 *)outcome);
        if (accepted && ((struct runtime_generic_ud_outcome_v1 *)outcome)->disposition ==
                RUNTIME_GENERIC_UD_STOP)
            runtime_mantle_generic_ud_stop_observation_mark();
        if (accepted && ((struct runtime_generic_ud_outcome_v1 *)outcome)->disposition ==
                RUNTIME_GENERIC_UD_PENDING)
            runtime_mantle_generic_ud_pending_observation_mark();
        return accepted;
    }
    return 0;
}
