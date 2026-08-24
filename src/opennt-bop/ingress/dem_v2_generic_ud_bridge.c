#include "dem_v2_generic_ud_bridge.h"
#include "opennt-bop/ingress/dem_native_session.h"

int runtime_dem_v2_generic_ud_recognizes(
    const struct runtime_generic_ud_event_v1 *event)
{
    return event != 0 && event->magic == RUNTIME_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes >= 4u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x50u;
}

int runtime_dem_v2_generic_ud_dispatch(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome)
{
    /* A valid DEM encoding is terminally owned here: an unbound/failed v2
     * session declines to the CPU, rather than falling through to legacy DEM
     * code.  All non-DEM traffic remains outside this DEM-only component. */
    return runtime_dem_v2_generic_ud_recognizes(event) ?
        runtime_dem_native_session_dispatch(event, outcome) : 0;
}
