#ifndef MVDM_SOFTPC_TERMINATION_H
#define MVDM_SOFTPC_TERMINATION_H

#include <stdint.h>

struct _EXCEPTION_POINTERS;

/* Same-shaped replacement for the selected NT4 product-shell terminal path.
 * It never terminates the application process: a bound session receives the
 * original VDM exit code and escapes to its app-owned execution boundary. */
int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code);

/* Diagnostic-only companion to the original top-level VDM exception filter.
 * It records the original exception identity without changing the filter's
 * result, selected termination path, or guest/CPU state. */
void mvdm_softpc_record_unhandled_exception(
    const struct _EXCEPTION_POINTERS *exception_info);

/* Fixed-container diagnostic only.  It observes the original top-level main
 * return after host_main/SEH have already selected the result. */
void mvdm_softpc_record_main_return(int result);

/* Fixed-container diagnostic only. `selector` and `service` were already
 * decoded by original MVDM code. The four guest words are copied at that
 * ingress and are observational only; this function neither routes nor
 * changes guest state. */
void mvdm_softpc_record_bop_dispatch(unsigned int selector,
                                     unsigned int service,
                                     unsigned int guest_cs,
                                     unsigned int guest_ip,
                                     unsigned int guest_ds,
                                     unsigned int guest_dx);

/* Fixed-container diagnostic only.  Called after the original BOP handler
 * and its original service-byte IP advance; copies already-live CPU words. */
void mvdm_softpc_record_bop_return(unsigned int selector,
                                   unsigned int service,
                                   unsigned int guest_cs,
                                   unsigned int guest_ip,
                                   unsigned int guest_ax,
                                   unsigned int guest_cf);

/* Fixed-container diagnostic only.  `stage` identifies an already-selected
 * original COMMAND call boundary; it does not route or alter that call. */
void mvdm_softpc_record_command_call(unsigned int service,
                                    unsigned int stage,
                                    unsigned int guest_ax,
                                    unsigned int guest_cf);

/* Default-off, fixed-container DEM observation.  The caller passes only the
 * original numeric DS:SI form; this helper takes and releases its own bounded
 * session lease before recording copied text and scalar outcome. */
void mvdm_softpc_record_dem_open(uint16_t guest_ds, uint16_t guest_si,
    unsigned int phase, unsigned int status, unsigned int guest_ax,
    unsigned int guest_cf);

#endif
