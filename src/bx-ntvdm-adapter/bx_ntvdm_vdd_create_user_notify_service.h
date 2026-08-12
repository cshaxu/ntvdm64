#ifndef BX_NTVDM_VDD_CREATE_USER_NOTIFY_SERVICE_H
#define BX_NTVDM_VDD_CREATE_USER_NOTIFY_SERVICE_H

#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_cpu_result_v2.h"

/* The contained profile admits no VDD registrations; the source handler list is empty. */
int bx_ntvdm_vdd_create_user_notify_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
