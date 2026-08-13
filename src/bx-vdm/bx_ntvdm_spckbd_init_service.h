#ifndef BX_NTVDM_SPCKBD_INIT_SERVICE_H
#define BX_NTVDM_SPCKBD_INIT_SERVICE_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Guest-visible completion of NTIO's BOP 5F SoftPC C-BIOS handoff. Native
 * Bochs owns PIC/PIT/8042 mechanics; this service owns only the source-defined
 * x86 continuation for the verified BEEF request. */
int bx_ntvdm_spckbd_init_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif
