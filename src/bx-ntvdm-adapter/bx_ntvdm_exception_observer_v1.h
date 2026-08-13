#ifndef BX_NTVDM_EXCEPTION_OBSERVER_V1_H
#define BX_NTVDM_EXCEPTION_OBSERVER_V1_H

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_instruction_window_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Adapter-owned, copied-fact observation.  The callback receives an opaque
 * diagnostic string only; it is not a guest-memory, provider, or CPU-result
 * channel and must not be retained by either side. */
typedef void (*bx_ntvdm_exception_observer_v1_report)(void *opaque,
    const char *message);

int bx_ntvdm_exception_observer_v1_observe(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_observer_v1_report report, void *opaque);

#ifdef __cplusplus
}
#endif

#endif
