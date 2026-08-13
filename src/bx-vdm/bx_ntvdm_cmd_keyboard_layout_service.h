#ifndef BX_NTVDM_CMD_KEYBOARD_LAYOUT_SERVICE_H
#define BX_NTVDM_CMD_KEYBOARD_LAYOUT_SERVICE_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

/* Fixed en-US/no-KB16 result. This is a command-profile policy, not a Bochs
 * keyboard or memory service. */
int bx_ntvdm_cmd_keyboard_layout_v1_dispatch(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
