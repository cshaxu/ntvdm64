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
static unsigned observed_bios15;
static uint32_t observed_bios15_eax;
static uint16_t observed_bios15_cs;
static uint32_t observed_bios15_eip;
static unsigned observed_bios15_next_byte;
static unsigned observed_bios15_composition_handled;
static unsigned observed_bios15_outcome;
static unsigned observed_ioctl;
static uint16_t observed_ioctl_cs;
static uint32_t observed_ioctl_eip;
static uint32_t observed_ioctl_eax, observed_ioctl_ebx, observed_ioctl_ecx, observed_ioctl_edx;
static unsigned observed_fast_read;
static uint16_t observed_fast_read_cs, observed_fast_read_ds, observed_fast_read_es;
static uint32_t observed_fast_read_eip, observed_fast_read_eax, observed_fast_read_ebx, observed_fast_read_ecx, observed_fast_read_edx, observed_fast_read_esi, observed_fast_read_edi;
static unsigned observed_spckbd;
static uint16_t observed_spckbd_cs, observed_spckbd_ds, observed_spckbd_es;
static uint32_t observed_spckbd_eip, observed_spckbd_eax, observed_spckbd_ebx,
    observed_spckbd_ecx, observed_spckbd_edx, observed_spckbd_esi,
    observed_spckbd_edi, observed_spckbd_eflags;
static unsigned observed_emm_probe;
static uint16_t observed_emm_probe_cs, observed_emm_probe_ds;
static uint32_t observed_emm_probe_eip, observed_emm_probe_eax,
    observed_emm_probe_ebx, observed_emm_probe_edx, observed_emm_probe_eflags;
static unsigned observed_mouse_install1;
static uint16_t observed_mouse_install1_cs, observed_mouse_install1_ds;
static uint32_t observed_mouse_install1_eip, observed_mouse_install1_eax,
    observed_mouse_install1_ebx, observed_mouse_install1_ecx,
    observed_mouse_install1_edx, observed_mouse_install1_eflags;
static unsigned observed_printer;
static uint16_t observed_printer_cs, observed_printer_ds;
static uint32_t observed_printer_eip, observed_printer_eax, observed_printer_ebx,
    observed_printer_ecx, observed_printer_edx, observed_printer_eflags;
static unsigned observed_config_done;
static uint16_t observed_config_done_cs, observed_config_done_ds;
static uint32_t observed_config_done_eip, observed_config_done_eax,
    observed_config_done_ebx, observed_config_done_ecx, observed_config_done_edx,
    observed_config_done_eflags;
static uint16_t observed_first_generic_cs;
static uint32_t observed_first_generic_eip;
static uint32_t observed_first_generic_mode;
static uint32_t observed_first_generic_vector;
static unsigned observed_first_generic_window_bytes;
static uint8_t observed_first_generic_window[4];
static unsigned observed_first_generic_history_count;
static struct bx_ntvdm_instruction_history_record_v1 observed_first_generic_history[8];
static unsigned observed_first_generic_cs_transition_valid;
static struct bx_ntvdm_instruction_history_transition_v1 observed_first_generic_cs_transition;
static unsigned observed_first_generic_cs_provenance_valid;
static struct bx_ntvdm_instruction_history_provenance_v1 observed_first_generic_cs_provenance;

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
        observed_first_generic_cs_transition_valid =
            bx_ntvdm_mantle_instruction_history_v1_get_latest_cs_transition(
                &observed_first_generic_cs_transition) ? 1u : 0u;
        observed_first_generic_cs_provenance_valid =
            bx_ntvdm_mantle_instruction_history_v1_get_latest_cs_provenance(
                &observed_first_generic_cs_provenance) ? 1u : 0u;
        outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
        outcome->disposition = BX_NTVDM_GENERIC_UD_STOP;
        observed_stop = 1u;
        return 1;
    }
    if (event != 0 && event->window_bytes >= 4u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == 0x50u &&
        event->window[3] == 0x11u) observed_5011 = 1u;
    if (bx_ntvdm_boot_namespace_composition_v1_handle(event, outcome)) {
        if (event != 0 && event->window_bytes >= 3u &&
            event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
            event->window[2] == 0x15u) {
            observed_bios15_composition_handled = 1u;
            observed_bios15_outcome = outcome != 0 ? outcome->disposition : 0u;
        }
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
        return 1;
    }
    if (event == 0 || outcome == 0) return 0;
    if (!observed_spckbd && event->window_bytes >= 3u &&
        event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
        event->window[2] == 0x5fu) {
        observed_spckbd = 1u; observed_spckbd_cs = event->cs;
        observed_spckbd_ds = event->ds; observed_spckbd_es = event->es;
        observed_spckbd_eip = event->eip; observed_spckbd_eax = event->eax;
        observed_spckbd_ebx = event->ebx; observed_spckbd_ecx = event->ecx;
        observed_spckbd_edx = event->edx; observed_spckbd_esi = event->esi;
        observed_spckbd_edi = event->edi; observed_spckbd_eflags = event->eflags;
    }
    if (!observed_emm_probe && event->window_bytes >= 3u &&
        event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
        event->window[2] == 0x66u) {
        observed_emm_probe = 1u; observed_emm_probe_cs = event->cs;
        observed_emm_probe_ds = event->ds; observed_emm_probe_eip = event->eip;
        observed_emm_probe_eax = event->eax; observed_emm_probe_ebx = event->ebx;
        observed_emm_probe_edx = event->edx; observed_emm_probe_eflags = event->eflags;
    }
    if (!observed_mouse_install1 && event->window_bytes >= 3u &&
        event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
        event->window[2] == 0xc8u) {
        observed_mouse_install1 = 1u; observed_mouse_install1_cs = event->cs;
        observed_mouse_install1_ds = event->ds; observed_mouse_install1_eip = event->eip;
        observed_mouse_install1_eax = event->eax; observed_mouse_install1_ebx = event->ebx;
        observed_mouse_install1_ecx = event->ecx; observed_mouse_install1_edx = event->edx;
        observed_mouse_install1_eflags = event->eflags;
    }
    if (!observed_printer && event->window_bytes >= 3u &&
        event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
        event->window[2] == 0x17u) {
        observed_printer = 1u; observed_printer_cs = event->cs;
        observed_printer_ds = event->ds; observed_printer_eip = event->eip;
        observed_printer_eax = event->eax; observed_printer_ebx = event->ebx;
        observed_printer_ecx = event->ecx; observed_printer_edx = event->edx;
        observed_printer_eflags = event->eflags;
    }
    if (!observed_config_done && event->window_bytes >= 3u &&
        event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
        event->window[2] == 0x5eu) {
        observed_config_done = 1u; observed_config_done_cs = event->cs;
        observed_config_done_ds = event->ds; observed_config_done_eip = event->eip;
        observed_config_done_eax = event->eax; observed_config_done_ebx = event->ebx;
        observed_config_done_ecx = event->ecx; observed_config_done_edx = event->edx;
        observed_config_done_eflags = event->eflags;
    }
    if (!observed_fast_read && event->window_bytes >= 4u &&
        event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
        event->window[2] == 0x50u && event->window[3] == 0x42u) {
        observed_fast_read = 1u; observed_fast_read_cs = event->cs;
        observed_fast_read_ds = event->ds; observed_fast_read_es = event->es;
        observed_fast_read_eip = event->eip; observed_fast_read_eax = event->eax;
        observed_fast_read_ebx = event->ebx; observed_fast_read_ecx = event->ecx;
        observed_fast_read_edx = event->edx; observed_fast_read_esi = event->esi;
        observed_fast_read_edi = event->edi;
    }
    if (!observed_ioctl && event->window_bytes >= 4u &&
        event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
        event->window[2] == 0x50u && event->window[3] == 0x21u) {
        observed_ioctl = 1u;
        observed_ioctl_cs = event->cs; observed_ioctl_eip = event->eip;
        observed_ioctl_eax = event->eax; observed_ioctl_ebx = event->ebx;
        observed_ioctl_ecx = event->ecx; observed_ioctl_edx = event->edx;
    }
    if (!observed_bios15 && event->window_bytes >= 3u &&
        event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
        event->window[2] == 0x15u) {
        observed_bios15 = 1u;
        observed_bios15_eax = event->eax;
        observed_bios15_cs = event->cs;
        observed_bios15_eip = event->eip;
        observed_bios15_next_byte = event->window_bytes >= 4u ? event->window[3] : 0u;
    }
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
unsigned t198_s23_native_ntio_boundary_observed_first_generic_cs_transition_valid(void) { return observed_first_generic_cs_transition_valid; }
int t198_s23_native_ntio_boundary_copy_first_generic_cs_transition(struct bx_ntvdm_instruction_history_transition_v1 *value) { if (!value || !observed_first_generic_cs_transition_valid) return 0; *value = observed_first_generic_cs_transition; return 1; }
unsigned t198_s23_native_ntio_boundary_observed_first_generic_cs_provenance_valid(void) { return observed_first_generic_cs_provenance_valid; }
int t198_s23_native_ntio_boundary_copy_first_generic_cs_provenance(struct bx_ntvdm_instruction_history_provenance_v1 *value) { if (!value || !observed_first_generic_cs_provenance_valid) return 0; *value = observed_first_generic_cs_provenance; return 1; }
unsigned t198_s23_native_ntio_boundary_observed_stop(void) { return observed_stop; }
unsigned t198_s23_native_ntio_boundary_observed_selector(void) { return observed_selector; }
unsigned t198_s23_native_ntio_boundary_observed_service(void) { return observed_service; }
unsigned t198_s23_native_ntio_boundary_observed_bios15(void) { return observed_bios15; }
unsigned t198_s23_native_ntio_boundary_observed_bios15_eax(void) { return observed_bios15_eax; }
unsigned t198_s23_native_ntio_boundary_observed_bios15_cs(void) { return observed_bios15_cs; }
unsigned t198_s23_native_ntio_boundary_observed_bios15_eip(void) { return observed_bios15_eip; }
unsigned t198_s23_native_ntio_boundary_observed_bios15_next_byte(void) { return observed_bios15_next_byte; }
unsigned t198_s23_native_ntio_boundary_observed_bios15_composition_handled(void) { return observed_bios15_composition_handled; }
unsigned t198_s23_native_ntio_boundary_observed_bios15_outcome(void) { return observed_bios15_outcome; }
unsigned t198_s23_native_ntio_boundary_observed_ioctl(void) { return observed_ioctl; }
unsigned t198_s23_native_ntio_boundary_observed_ioctl_cs(void) { return observed_ioctl_cs; }
unsigned t198_s23_native_ntio_boundary_observed_ioctl_eip(void) { return observed_ioctl_eip; }
unsigned t198_s23_native_ntio_boundary_observed_ioctl_eax(void) { return observed_ioctl_eax; }
unsigned t198_s23_native_ntio_boundary_observed_ioctl_ebx(void) { return observed_ioctl_ebx; }
unsigned t198_s23_native_ntio_boundary_observed_ioctl_ecx(void) { return observed_ioctl_ecx; }
unsigned t198_s23_native_ntio_boundary_observed_ioctl_edx(void) { return observed_ioctl_edx; }
unsigned t198_s23_native_ntio_boundary_observed_fast_read(void) { return observed_fast_read; }
unsigned t198_s23_native_ntio_boundary_observed_fast_read_cs(void) { return observed_fast_read_cs; }
unsigned t198_s23_native_ntio_boundary_observed_fast_read_ds(void) { return observed_fast_read_ds; }
unsigned t198_s23_native_ntio_boundary_observed_fast_read_es(void) { return observed_fast_read_es; }
unsigned t198_s23_native_ntio_boundary_observed_fast_read_eip(void) { return observed_fast_read_eip; }
unsigned t198_s23_native_ntio_boundary_observed_fast_read_eax(void) { return observed_fast_read_eax; }
unsigned t198_s23_native_ntio_boundary_observed_fast_read_ebx(void) { return observed_fast_read_ebx; }
unsigned t198_s23_native_ntio_boundary_observed_fast_read_ecx(void) { return observed_fast_read_ecx; }
unsigned t198_s23_native_ntio_boundary_observed_fast_read_edx(void) { return observed_fast_read_edx; }
unsigned t198_s23_native_ntio_boundary_observed_fast_read_esi(void) { return observed_fast_read_esi; }
unsigned t198_s23_native_ntio_boundary_observed_fast_read_edi(void) { return observed_fast_read_edi; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd(void) { return observed_spckbd; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd_cs(void) { return observed_spckbd_cs; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd_ds(void) { return observed_spckbd_ds; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd_es(void) { return observed_spckbd_es; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd_eip(void) { return observed_spckbd_eip; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd_eax(void) { return observed_spckbd_eax; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd_ebx(void) { return observed_spckbd_ebx; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd_ecx(void) { return observed_spckbd_ecx; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd_edx(void) { return observed_spckbd_edx; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd_esi(void) { return observed_spckbd_esi; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd_edi(void) { return observed_spckbd_edi; }
unsigned t198_s23_native_ntio_boundary_observed_spckbd_eflags(void) { return observed_spckbd_eflags; }
unsigned t198_s23_native_ntio_boundary_observed_emm_probe(void) { return observed_emm_probe; }
unsigned t198_s23_native_ntio_boundary_observed_emm_probe_cs(void) { return observed_emm_probe_cs; }
unsigned t198_s23_native_ntio_boundary_observed_emm_probe_ds(void) { return observed_emm_probe_ds; }
unsigned t198_s23_native_ntio_boundary_observed_emm_probe_eip(void) { return observed_emm_probe_eip; }
unsigned t198_s23_native_ntio_boundary_observed_emm_probe_eax(void) { return observed_emm_probe_eax; }
unsigned t198_s23_native_ntio_boundary_observed_emm_probe_ebx(void) { return observed_emm_probe_ebx; }
unsigned t198_s23_native_ntio_boundary_observed_emm_probe_edx(void) { return observed_emm_probe_edx; }
unsigned t198_s23_native_ntio_boundary_observed_emm_probe_eflags(void) { return observed_emm_probe_eflags; }
unsigned t198_s23_native_ntio_boundary_observed_mouse_install1(void) { return observed_mouse_install1; }
unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_cs(void) { return observed_mouse_install1_cs; }
unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_ds(void) { return observed_mouse_install1_ds; }
unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_eip(void) { return observed_mouse_install1_eip; }
unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_eax(void) { return observed_mouse_install1_eax; }
unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_ebx(void) { return observed_mouse_install1_ebx; }
unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_ecx(void) { return observed_mouse_install1_ecx; }
unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_edx(void) { return observed_mouse_install1_edx; }
unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_eflags(void) { return observed_mouse_install1_eflags; }
unsigned t198_s23_native_ntio_boundary_observed_printer(void) { return observed_printer; }
unsigned t198_s23_native_ntio_boundary_observed_printer_cs(void) { return observed_printer_cs; }
unsigned t198_s23_native_ntio_boundary_observed_printer_ds(void) { return observed_printer_ds; }
unsigned t198_s23_native_ntio_boundary_observed_printer_eip(void) { return observed_printer_eip; }
unsigned t198_s23_native_ntio_boundary_observed_printer_eax(void) { return observed_printer_eax; }
unsigned t198_s23_native_ntio_boundary_observed_printer_ebx(void) { return observed_printer_ebx; }
unsigned t198_s23_native_ntio_boundary_observed_printer_ecx(void) { return observed_printer_ecx; }
unsigned t198_s23_native_ntio_boundary_observed_printer_edx(void) { return observed_printer_edx; }
unsigned t198_s23_native_ntio_boundary_observed_printer_eflags(void) { return observed_printer_eflags; }
unsigned t198_s23_native_ntio_boundary_observed_config_done(void) { return observed_config_done; }
unsigned t198_s23_native_ntio_boundary_observed_config_done_cs(void) { return observed_config_done_cs; }
unsigned t198_s23_native_ntio_boundary_observed_config_done_ds(void) { return observed_config_done_ds; }
unsigned t198_s23_native_ntio_boundary_observed_config_done_eip(void) { return observed_config_done_eip; }
unsigned t198_s23_native_ntio_boundary_observed_config_done_eax(void) { return observed_config_done_eax; }
unsigned t198_s23_native_ntio_boundary_observed_config_done_ebx(void) { return observed_config_done_ebx; }
unsigned t198_s23_native_ntio_boundary_observed_config_done_ecx(void) { return observed_config_done_ecx; }
unsigned t198_s23_native_ntio_boundary_observed_config_done_edx(void) { return observed_config_done_edx; }
unsigned t198_s23_native_ntio_boundary_observed_config_done_eflags(void) { return observed_config_done_eflags; }
