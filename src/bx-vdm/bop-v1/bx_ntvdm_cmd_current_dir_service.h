#ifndef BX_NTVDM_CMD_CURRENT_DIR_SERVICE_H
#define BX_NTVDM_CMD_CURRENT_DIR_SERVICE_H

#include <stdint.h>

#include "bx_ntvdm_command_host_context_v1.h"
#include "bx_ntvdm_multi_write_transaction.h"

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_cmd_current_dir_service_v1_prepare(uint32_t available_mask,
    const bx_ntvdm_command_host_context_v1 *host_context,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD]);
#ifdef __cplusplus
}
#endif

#endif
