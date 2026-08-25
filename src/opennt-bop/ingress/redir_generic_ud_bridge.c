#include "redir_generic_ud_bridge.h"
#include "opennt-bop/ingress/redir_native_session.h"

int runtime_redir_generic_ud_recognizes(
    const struct runtime_generic_ud_event *event)
{
    return event != NULL && event->magic == RUNTIME_GENERIC_UD_EVENT_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes >= 4u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x57u &&
        event->window[3] <= 0x31u;
}

int runtime_redir_generic_ud_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    return runtime_redir_generic_ud_recognizes(event) ?
        runtime_redir_native_session_dispatch(event, outcome) : 0;
}
