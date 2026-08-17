#ifndef BX_NTVDM_CMD_GET_NEXT_SERVICE_H
#define BX_NTVDM_CMD_GET_NEXT_SERVICE_H

#include "bx_ntvdm_cmd_set_info_service.h"
#include "bx_ntvdm_cmdinfo_v1.h"
#include "bx_ntvdm_guest_gather_read_action_v1.h"
#include "bx_ntvdm_host_drive_policy.h"
#include "bx_ntvdm_multi_write_transaction.h"
#include "bx_ntvdm_readonly_namespace.h"
#include "byob_launch_plan_v2.h"

#define BX_NTVDM_CMD_TERMINAL_RECORD_V1_MAGIC UINT32_C(0x42584354)
#define BX_NTVDM_CMD_TERMINAL_RECORD_V1_VERSION UINT32_C(1)

typedef enum bx_ntvdm_cmd_terminal_reason_v1 {
    BX_NTVDM_CMD_TERMINAL_REASON_V1_NONE = 0,
    BX_NTVDM_CMD_TERMINAL_REASON_V1_DECLARED_PLAN_EXHAUSTED = 1
} bx_ntvdm_cmd_terminal_reason_v1;

/* A copied package result.  It is deliberately not an engine result and
 * contains neither CPU state nor guest/host references. */
typedef struct bx_ntvdm_cmd_terminal_record_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t present;
    uint32_t reason;
    uint32_t dos_exit_code;
    uint32_t reserved0;
    uint32_t reserved1;
} bx_ntvdm_cmd_terminal_record_v1;

/* Each committed 54:01 consumes one immutable slot.  It does not imply a
 * 54:11 result, a host queue, or a guest pointer retained by the adapter. */
typedef struct bx_ntvdm_cmd_get_next_state_v1 {
    uint32_t delivered;
    uint32_t returned;
    uint16_t terminal_dos_exit_code;
    uint16_t reserved0;
    bx_ntvdm_cmd_terminal_record_v1 terminal;
} bx_ntvdm_cmd_get_next_state_v1;

void bx_ntvdm_cmd_get_next_state_v1_initialize(bx_ntvdm_cmd_get_next_state_v1 *value);
int bx_ntvdm_cmd_terminal_record_v1_valid(const bx_ntvdm_cmd_terminal_record_v1 *value);
int bx_ntvdm_cmd_get_next_terminal_v1_copy(const bx_ntvdm_cmd_get_next_state_v1 *state,
    bx_ntvdm_cmd_terminal_record_v1 *out);
/* Opaque COMMAND completion fact.  It is true only after the source-shaped
 * no-next-command return has recorded one valid exhausted-plan result. */
int bx_ntvdm_cmd_get_next_ordinary_completion_v1(
    const bx_ntvdm_cmd_get_next_state_v1 *state);
int bx_ntvdm_cmd_get_next_v1_prepare(const bx_ntvdm_cmd_get_next_state_v1 *state,
    const byob_launch_plan_v2 *plan,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_cmd_get_next_v1_complete(const bx_ntvdm_readonly_namespace_v1 *ns,
    const byob_launch_plan_v2 *plan, const bx_ntvdm_host_drive_snapshot_v1 *drives,
    uint32_t initial_environment_bytes,
    const bx_ntvdm_cmd_set_info_registration_v1 *registration,
    const bx_ntvdm_cmd_get_next_state_v1 *state, const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count, bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD]);
void bx_ntvdm_cmd_get_next_state_v1_commit(bx_ntvdm_cmd_get_next_state_v1 *state);
int bx_ntvdm_cmd_return_exit_code_v1_dispatch(
    bx_ntvdm_cmd_get_next_state_v1 *state,
    const byob_launch_plan_v2 *plan,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
