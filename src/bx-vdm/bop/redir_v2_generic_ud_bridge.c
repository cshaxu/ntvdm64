#include "redir_v2_generic_ud_bridge.h"
#include "shim/redir_session_shim.h"

int bx_ntvdm_redir_v2_generic_ud_recognizes(
    const struct bx_ntvdm_generic_ud_event_v1 *event)
{
    return event != NULL && event->magic == BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes >= 4u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x57u &&
        event->window[3] <= 0x31u;
}

int bx_ntvdm_redir_v2_generic_ud_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    return bx_ntvdm_redir_v2_generic_ud_recognizes(event) ?
        bx_ntvdm_redir_native_session_dispatch(event, outcome) : 0;
}
