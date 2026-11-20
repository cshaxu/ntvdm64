#ifndef MVDM_SOFTPC_TERMINATION_H
#define MVDM_SOFTPC_TERMINATION_H

#include <stdint.h>

/* Close the original machine and return to the armed host stack. */
int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code);

#endif
