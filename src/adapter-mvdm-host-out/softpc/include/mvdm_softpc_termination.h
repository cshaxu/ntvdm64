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

/* Fixed-container diagnostic only.  `selector` and `service` were already
 * decoded by original MVDM code; this records them without routing them. */
void mvdm_softpc_record_bop_dispatch(unsigned int selector,
                                     unsigned int service);

#endif
