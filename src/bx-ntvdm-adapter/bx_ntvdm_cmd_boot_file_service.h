#ifndef BX_NTVDM_CMD_BOOT_FILE_SERVICE_H
#define BX_NTVDM_CMD_BOOT_FILE_SERVICE_H

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_instruction_window_abi.h"
#include "bx_ntvdm_multi_write_transaction.h"
#include "bx_ntvdm_readonly_namespace.h"

/* Implements only COMMAND's two boot-file pathname services (54:0c/0d).
 * The returned OEM pathname names an already profile-materialized guest file;
 * it is not a Win32 path and does not allocate a host temporary file. */
int bx_ntvdm_cmd_boot_file_service_v1_prepare(
    const bx_ntvdm_readonly_namespace_v1 *namespace_value,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD]);

#endif
