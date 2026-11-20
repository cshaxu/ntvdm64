#ifndef COMPAT_MVDM_SOFTPC_OBSERVATION_H
#define COMPAT_MVDM_SOFTPC_OBSERVATION_H

/* Existing default-off observations still used by selected product callers. */
void mvdm_softpc_report_wow_fast_callback_binding(char const *stage,
    unsigned long address);
void mvdm_softpc_report_dpmi_mode_state(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip,
    unsigned long ds_base, unsigned long ds_limit);
void mvdm_softpc_report_cpu40_descriptor_publish(unsigned short selector,
    unsigned long source_low, unsigned long source_high,
    unsigned long gdt_low, unsigned long gdt_high, unsigned long ldt_low,
    unsigned long ldt_high);

#endif
