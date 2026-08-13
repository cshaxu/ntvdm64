#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"

static unsigned observed_5011;
static unsigned observed_stop;

int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (event != 0 && event->window_bytes >= 4u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x50u &&
        event->window[3] == 0x11u) observed_5011 = 1u;
    if (bx_ntvdm_boot_namespace_composition_v1_handle(event, outcome)) return 1;
    if (event == 0 || outcome == 0) return 0;
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = BX_NTVDM_GENERIC_UD_STOP;
    observed_stop = 1u;
    return 1;
}

unsigned t198_s23_native_ntio_boundary_observed_5011(void) { return observed_5011; }
unsigned t198_s23_native_ntio_boundary_observed_stop(void) { return observed_stop; }
