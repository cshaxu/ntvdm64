#ifndef BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_SERVICE_H
#define BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_SERVICE_H

#include <stdint.h>
#include "bx_ntvdm_guest_gather_read_action_v1.h"
#include "bx_ntvdm_multi_write_transaction.h"
#include "bx_ntvdm_readonly_namespace.h"

#define BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_MAX_BYTES 80u
#define BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES 64u
typedef struct bx_ntvdm_cmd_comspec_bootstrap_v1 {
    uint8_t environment[BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_MAX_BYTES];
    uint32_t environment_bytes;
    uint32_t ready;
} bx_ntvdm_cmd_comspec_bootstrap_v1;

void bx_ntvdm_cmd_comspec_bootstrap_v1_initialize(bx_ntvdm_cmd_comspec_bootstrap_v1 *value);
int bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_comspec(const bx_ntvdm_readonly_namespace_v1 *ns,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    const bx_ntvdm_cmd_comspec_bootstrap_v1 *state,
    bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_cmd_comspec_bootstrap_v1_complete_comspec(const bx_ntvdm_readonly_namespace_v1 *ns,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_gather_read_action_v1 *action, const uint8_t *bytes,
    uint64_t byte_count, bx_ntvdm_cmd_comspec_bootstrap_v1 *state,
    bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_environment(
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, const bx_ntvdm_cmd_comspec_bootstrap_v1 *state,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD]);

#endif
