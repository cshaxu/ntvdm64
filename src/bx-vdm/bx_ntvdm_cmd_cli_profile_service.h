#ifndef BX_NTVDM_CMD_CLI_PROFILE_SERVICE_H
#define BX_NTVDM_CMD_CLI_PROFILE_SERVICE_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

/*
 * Source-derived COMMAND services whose original implementation is a small
 * host-profile decision, rather than an NT VDD, Win32 handle, or CCPU/SAS
 * operation.  The contained CLI profile has one DOS execution domain and no
 * separately created console.  This is deliberately a COMMAND provider: it
 * contains neither Bochs nor generic BOP routing knowledge.
 */
int bx_ntvdm_cmd_cli_profile_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
