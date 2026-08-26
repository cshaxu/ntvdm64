#include "xms_entry.h"
#include "app/bop/xms_session.h"
#include "opennt-bop/xms/opennt_xms_compat.h"

int runtime_xms_generic_ud_recognizes(
    const struct runtime_generic_ud_event *event)
{
    return event != NULL && event->magic == RUNTIME_GENERIC_UD_EVENT_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes >= 4u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x52u &&
        event->window[3] < XMS_LASTSVC;
}

int runtime_xms_generic_ud_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    return runtime_xms_generic_ud_recognizes(event) ?
        runtime_xms_native_session_dispatch(event, outcome) : 0;
}
