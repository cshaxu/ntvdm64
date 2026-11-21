#ifndef MVDM_SOFTPC_TERMINATION_H
#define MVDM_SOFTPC_TERMINATION_H

#include <stdint.h>

/* Install the process-lifetime original close provider before host entry.
 * Binding again to the same provider is allowed; replacement is not. */
int mvdm_softpc_termination_bind(void (*close_host)(void));

/* Close the original machine and return to the armed host stack. */
int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code);

#endif
