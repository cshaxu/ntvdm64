#ifndef MVDM_SOFTPC_TERMINATION_H
#define MVDM_SOFTPC_TERMINATION_H

#include <stdint.h>

/* Same-shaped replacement for the selected NT4 product-shell terminal path.
 * It never terminates the application process: a bound session receives the
 * original VDM exit code and escapes to its app-owned execution boundary. */
int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code);

/* Default-off test attribution for an existing original TerminateVDM path. */
void mvdm_softpc_report_termination(char const *stage,
    void const *return_address);
void mvdm_softpc_report_virtual_memory_failure(unsigned long status);
void mvdm_softpc_report_wow_bop_state(unsigned long cr0, unsigned long cr3,
    unsigned short cs, unsigned short ip, unsigned long ds_base,
    unsigned long ds_limit);
/* Default-off post-dispatch observation.  It is emitted only after the
 * original W32Dispatch returns to its BOP owner. */
void mvdm_softpc_report_wow_dispatch_return_state(unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip);
void mvdm_softpc_report_dpmi_mode_state(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip,
    unsigned long ds_base, unsigned long ds_limit);
/* Default-off descriptor-publication witness for the bounded WOW LDT alias
 * investigation.  The caller supplies records it has already published; the
 * witness does not inspect or modify guest state. */
void mvdm_softpc_report_dpmi_descriptor(unsigned short selector,
    unsigned long source_low, unsigned long source_high,
    unsigned long gdt_low, unsigned long gdt_high, unsigned long ldt_low,
    unsigned long ldt_high);
/* Default-off witness for the original 53:00 descriptor-array conversion.
 * It captures only the BOP's existing register inputs and converted address,
 * before the original function reads the first descriptor. */
void mvdm_softpc_report_dpmi_set_descriptor(unsigned short es,
    unsigned short bx, unsigned short ax, unsigned short cx,
    unsigned long descriptor_pointer, unsigned long protected_mode);

#endif
