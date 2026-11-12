#ifndef MVDM_SOFTPC_TERMINATION_H
#define MVDM_SOFTPC_TERMINATION_H

#include <stdint.h>

/* End the bound session through its original host close and app-owned escape. */
int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code);

/* Existing default-off observations still used by selected product callers. */
void mvdm_softpc_report_xms_move(unsigned long source,
    unsigned long destination, unsigned long byte_count, int completed);
void mvdm_softpc_report_wow_bop_state(unsigned long cr0, unsigned long cr3,
    unsigned short cs, unsigned short ip, unsigned long ds_base,
    unsigned long ds_limit);
void mvdm_softpc_report_wow_stage(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip);
void mvdm_softpc_report_wow_fast_callback_binding(char const *stage,
    unsigned long address);
void mvdm_softpc_report_dpmi_mode_state(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip,
    unsigned long ds_base, unsigned long ds_limit);
void mvdm_softpc_report_cpu40_descriptor_publish(unsigned short selector,
    unsigned long source_low, unsigned long source_high,
    unsigned long gdt_low, unsigned long gdt_high, unsigned long ldt_low,
    unsigned long ldt_high);

/* A stale NIDDB identity cannot safely select a device instance. */
__declspec(noreturn) void mvdm_softpc_invalid_virtual_instance(unsigned long id);

#endif
