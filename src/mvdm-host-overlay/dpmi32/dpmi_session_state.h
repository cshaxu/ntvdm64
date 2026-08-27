#ifndef MVDM_HOST_OVERLAY_DPMI_SESSION_STATE_H
#define MVDM_HOST_OVERLAY_DPMI_SESSION_STATE_H

#include <stdint.h>

typedef struct mvdm_dpmi_session_state_snapshot {
    uint16_t dosx_stack_segment;
    uint16_t dosx_stack_frame_size;
    uint16_t dosx_rm_code_segment;
    uint16_t dosx_pm_data_selector;
    uint16_t dosx_rm_code_selector;
    uint16_t current_app_flags;
    uint16_t current_dta_offset;
    uint16_t current_dta_selector;
    uint16_t current_psp_selector;
    uint32_t small_xlat_buffer;
    uint32_t large_xlat_buffer;
    uint32_t dosx_stack_frame;
    uint32_t rm_bop_fe;
    uint32_t dosx_dta_buffer;
    uint32_t dosx_segment_to_selector;
    uint32_t dosx_fault_iret;
    uint32_t dosx_fault_iretd;
    uint32_t dosx_int_iret;
    uint32_t dosx_int_iretd;
    uint32_t dosx_iret;
    uint32_t dosx_iretd;
    uint32_t selector_table;
    uint32_t current_dta;
} mvdm_dpmi_session_state_snapshot;

int mvdm_dpmi_session_record_dosx(const uint8_t *bytes, uint32_t byte_count);
int mvdm_dpmi_session_record_app(uint16_t flags, uint32_t dta,
    uint16_t dta_offset, uint16_t dta_selector, uint16_t psp_selector);
int mvdm_dpmi_session_record_selector_table(uint16_t selector);
int mvdm_dpmi_session_snapshot(mvdm_dpmi_session_state_snapshot *snapshot_out);

#endif
