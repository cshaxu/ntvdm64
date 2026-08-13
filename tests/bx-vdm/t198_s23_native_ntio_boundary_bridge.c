#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx-mantle/bx_ntvdm_instruction_history.h"

static unsigned observed_5011;
static unsigned observed_503b_resume;
static unsigned observed_dta_resume;
static unsigned observed_hard_error_resume;
static unsigned observed_system_symbol_resume;
static unsigned observed_drive_resume;
static unsigned observed_first_generic_ud;
static struct bx_ntvdm_generic_ud_event_v1 observed_first_generic_event;
static unsigned observed_stop;
static unsigned observed_selector;
static unsigned observed_service;
static uint16_t observed_first_generic_cs;
static uint32_t observed_first_generic_eip;
static uint32_t observed_first_generic_mode;
static uint32_t observed_first_generic_vector;
static unsigned observed_first_generic_window_bytes;
static uint8_t observed_first_generic_window[4];
static unsigned observed_first_generic_history_count;
static struct bx_ntvdm_instruction_history_record_v1 observed_first_generic_history[8];

int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (event != 0 && outcome != 0 && !observed_first_generic_ud &&
        !(event->window_bytes >= 2u && event->window[0] == 0xc4u &&
          event->window[1] == 0xc4u)) {
        unsigned count = event->window_bytes < sizeof(observed_first_generic_window) ?
            event->window_bytes : sizeof(observed_first_generic_window);
        unsigned i;
        observed_first_generic_ud = 1u;
        observed_first_generic_event = *event;
        observed_first_generic_cs = event->cs;
        observed_first_generic_eip = event->eip;
        observed_first_generic_mode = event->execution_mode;
        observed_first_generic_vector = event->vector;
        observed_first_generic_window_bytes = event->window_bytes;
        for (i = 0; i < count; ++i) observed_first_generic_window[i] = event->window[i];
        observed_first_generic_history_count = bx_ntvdm_mantle_instruction_history_v1_count();
        if (observed_first_generic_history_count > 8u)
            observed_first_generic_history_count = 8u;
        for (i = 0; i < observed_first_generic_history_count; ++i) {
            if (!bx_ntvdm_mantle_instruction_history_v1_get(i,
                &observed_first_generic_history[i])) {
                observed_first_generic_history_count = 0u;
                break;
            }
        }
        outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
        outcome->disposition = BX_NTVDM_GENERIC_UD_STOP;
        observed_stop = 1u;
        return 1;
    }
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
        if (event != 0 && outcome != 0 && event->window_bytes >= 4u &&
            event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
            event->window[2] == 0x50u && event->window[3] == 0x32u &&
            outcome->disposition == BX_NTVDM_GENERIC_UD_RESUME &&
            outcome->resume_rip == event->fault_rip + 4u)
            observed_hard_error_resume = 1u;
        if (event != 0 && outcome != 0 && event->window_bytes >= 4u &&
            event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
            event->window[2] == 0x50u && event->window[3] == 0x45u &&
            outcome->disposition == BX_NTVDM_GENERIC_UD_RESUME &&
            outcome->resume_rip == event->fault_rip + 4u)
            observed_system_symbol_resume = 1u;
        if (event != 0 && outcome != 0 && event->window_bytes >= 4u &&
            event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
            event->window[2] == 0x50u && event->window[3] == 0x0fu &&
            outcome->disposition == BX_NTVDM_GENERIC_UD_RESUME &&
            outcome->resume_rip == event->fault_rip + 4u)
            observed_drive_resume = 1u;
        if (observed_drive_resume) {
            outcome->disposition = BX_NTVDM_GENERIC_UD_STOP;
            observed_stop = 1u;
        }
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
unsigned t198_s23_native_ntio_boundary_observed_hard_error_resume(void) { return observed_hard_error_resume; }
unsigned t198_s23_native_ntio_boundary_observed_system_symbol_resume(void) { return observed_system_symbol_resume; }
unsigned t198_s23_native_ntio_boundary_observed_drive_resume(void) { return observed_drive_resume; }
unsigned t198_s23_native_ntio_boundary_observed_first_generic_ud(void) { return observed_first_generic_ud; }
int t198_s23_native_ntio_boundary_copy_first_generic_event(struct bx_ntvdm_generic_ud_event_v1 *value) { if (!value || !observed_first_generic_ud) return 0; *value = observed_first_generic_event; return 1; }
unsigned t198_s23_native_ntio_boundary_observed_first_generic_cs(void) { return observed_first_generic_cs; }
unsigned t198_s23_native_ntio_boundary_observed_first_generic_eip(void) { return observed_first_generic_eip; }
unsigned t198_s23_native_ntio_boundary_observed_first_generic_mode(void) { return observed_first_generic_mode; }
unsigned t198_s23_native_ntio_boundary_observed_first_generic_vector(void) { return observed_first_generic_vector; }
unsigned t198_s23_native_ntio_boundary_observed_first_generic_window_bytes(void) { return observed_first_generic_window_bytes; }
unsigned t198_s23_native_ntio_boundary_observed_first_generic_window(unsigned index) { return index < sizeof(observed_first_generic_window) ? observed_first_generic_window[index] : 0u; }
unsigned t198_s23_native_ntio_boundary_observed_first_generic_history_count(void) { return observed_first_generic_history_count; }
int t198_s23_native_ntio_boundary_copy_first_generic_history(unsigned index, struct bx_ntvdm_instruction_history_record_v1 *value) { if (!value || index >= observed_first_generic_history_count) return 0; *value = observed_first_generic_history[index]; return 1; }
unsigned t198_s23_native_ntio_boundary_observed_stop(void) { return observed_stop; }
unsigned t198_s23_native_ntio_boundary_observed_selector(void) { return observed_selector; }
unsigned t198_s23_native_ntio_boundary_observed_service(void) { return observed_service; }
