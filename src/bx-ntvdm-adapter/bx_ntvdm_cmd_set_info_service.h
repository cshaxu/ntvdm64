#ifndef BX_NTVDM_CMD_SET_INFO_SERVICE_H
#define BX_NTVDM_CMD_SET_INFO_SERVICE_H

#include <stdint.h>
#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

typedef struct bx_ntvdm_cmd_set_info_registration_v1 {
    uint64_t scs_info;
    uint64_t scs_to_sync;
    uint64_t is_dos_binary;
    uint64_t fd_access;
} bx_ntvdm_cmd_set_info_registration_v1;

int bx_ntvdm_cmd_set_info_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cmd_set_info_registration_v1 *registration,
    bx_ntvdm_cpu_result_v2 *result);

#endif
