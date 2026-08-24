#include "top_level_nosupport_v2_generic_ud_bridge.h"
#include "adapter-win32/facade/opennt_error_dialog_facade.h"
#include "opennt-host/softpc.new/host/inc/error.h"

#include <string.h>

int bx_ntvdm_top_level_nosupport_v2_generic_ud_recognizes(
    const struct bx_ntvdm_generic_ud_event_v1 *event)
{
    return event != 0 && event->magic == BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x59u;
}

int bx_ntvdm_top_level_nosupport_v2_generic_ud_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (!bx_ntvdm_top_level_nosupport_v2_generic_ud_recognizes(event) ||
        outcome == 0 || event->fault_rip > UINT64_MAX - 3u) return 0;
    /* Direct mirror of MS_bop_9: consume AX as the direct-access category.
     * The shim retains nt_error.c's per-thread category suppression and its
     * Abort/Ignore choice through public Win32 UI. */
    host_direct_access_error((unsigned long)(event->eax & 0xffffu));
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    if (bx_ntvdm_opennt_direct_access_last_choice() ==
        BX_NTVDM_OPENNT_DIRECT_ACCESS_CHOICE_V1_TERMINATE) {
        outcome->disposition = BX_NTVDM_GENERIC_UD_STOP;
        return 1;
    }
    outcome->disposition = BX_NTVDM_GENERIC_UD_RESUME;
    outcome->resume_rip = event->fault_rip + 3u;
    return 1;
}
