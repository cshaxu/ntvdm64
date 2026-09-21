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
void mvdm_softpc_report_dpmi_mode_state(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip,
    unsigned long ds_base, unsigned long ds_limit);

#endif
