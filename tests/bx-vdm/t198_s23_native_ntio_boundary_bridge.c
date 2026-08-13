#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"

static unsigned observed_5011;
static unsigned observed_503b_resume;
static unsigned observed_dta_resume;
static unsigned observed_stop;
static unsigned observed_selector;
static unsigned observed_service;

int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (event != 0 && event->window_bytes >= 4u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x50u &&
        event->window[3] == 0x11u) observed_5011 = 1u;
    if (bx_ntvdm_boot_namespace_composition_v1_handle(event, outcome)) {
        if (event != 0 && outcome != 0 && event->window_bytes >= 4u &&
            event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
            event->window[2] == 0x50u && event->window[3] == 0x3bu &&
            outcome->disposition == BX_NTVDM_GENERIC_UD_RESUME &&
            outcome->resume_rip == event->fault_rip + 4u)
            observed_503b_resume = 1u;
        if (event != 0 && outcome != 0 && event->window_bytes >= 4u &&
            event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
            event->window[2] == 0x50u && event->window[3] == 0x1bu &&
            outcome->disposition == BX_NTVDM_GENERIC_UD_RESUME &&
            outcome->resume_rip == event->fault_rip + 4u)
            observed_dta_resume = 1u;
        return 1;
    }
    if (event == 0 || outcome == 0) return 0;
    if (event->window_bytes >= 4u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u) {
        observed_selector = event->window[2];
        observed_service = event->window[3];
    }
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = BX_NTVDM_GENERIC_UD_STOP;
    observed_stop = 1u;
    return 1;
}

unsigned t198_s23_native_ntio_boundary_observed_5011(void) { return observed_5011; }
unsigned t198_s23_native_ntio_boundary_observed_503b_resume(void) { return observed_503b_resume; }
unsigned t198_s23_native_ntio_boundary_observed_dta_resume(void) { return observed_dta_resume; }
unsigned t198_s23_native_ntio_boundary_observed_stop(void) { return observed_stop; }
unsigned t198_s23_native_ntio_boundary_observed_selector(void) { return observed_selector; }
unsigned t198_s23_native_ntio_boundary_observed_service(void) { return observed_service; }
