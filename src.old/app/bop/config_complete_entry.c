#include "config_complete_entry.h"
#include "app/bop/dem_session.h"

void MS_bop_E(void);

int runtime_config_complete_generic_ud_recognizes(
    const struct runtime_generic_ud_event *event)
{
    return event != NULL && event->magic == RUNTIME_GENERIC_UD_EVENT_MAGIC &&
        event->abi_version == RUNTIME_GENERIC_UD_EVENT_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->execution_mode == RUNTIME_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x5eu;
}

int runtime_config_complete_generic_ud_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome)
{
    if (!runtime_config_complete_generic_ud_recognizes(event)) return 0;
    return runtime_dem_native_session_invoke_scoped_body(event, outcome,
        MS_bop_E, 3u);
}
