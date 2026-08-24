#include "config_complete_v2_generic_ud_bridge.h"
#include "opennt-bop/ingress/dem_native_session.h"

void MS_bop_E(void);

int bx_ntvdm_config_complete_v2_generic_ud_recognizes(
    const struct bx_ntvdm_generic_ud_event_v1 *event)
{
    return event != NULL && event->magic == BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x5eu;
}

int bx_ntvdm_config_complete_v2_generic_ud_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (!bx_ntvdm_config_complete_v2_generic_ud_recognizes(event)) return 0;
    return bx_ntvdm_dem_native_session_invoke_scoped_body(event, outcome,
        MS_bop_E, 3u);
}
