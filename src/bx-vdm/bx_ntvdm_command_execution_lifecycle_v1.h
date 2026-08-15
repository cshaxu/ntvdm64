#ifndef BX_NTVDM_COMMAND_EXECUTION_LIFECYCLE_V1_H
#define BX_NTVDM_COMMAND_EXECUTION_LIFECYCLE_V1_H

#include "bx_ntvdm_cmd_get_next_service.h"

#define BX_NTVDM_COMMAND_EXECUTION_LIFECYCLE_V1_MAGIC 0x42584345u
#define BX_NTVDM_COMMAND_EXECUTION_LIFECYCLE_V1_VERSION 1u

/* One COMMAND-owned lifecycle boundary for cmdCheckBinary, cmdExec,
 * cmdExecComspec32 and cmdReturnExitCode.  It deliberately owns no process,
 * handle, guest pointer or stream: those are admitted only when the shared
 * opaque stream and typed-child capability exists. */
typedef struct bx_ntvdm_command_execution_lifecycle_v1 {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t deferred_execution_count, reserved0;
} bx_ntvdm_command_execution_lifecycle_v1;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_command_execution_lifecycle_v1_initialize(
    bx_ntvdm_command_execution_lifecycle_v1 *value);
int bx_ntvdm_command_execution_lifecycle_v1_valid(
    const bx_ntvdm_command_execution_lifecycle_v1 *value);
int bx_ntvdm_command_execution_lifecycle_v1_dispatch(
    bx_ntvdm_command_execution_lifecycle_v1 *value,
    bx_ntvdm_cmd_get_next_state_v1 *get_next,
    const byob_launch_plan_v2 *launch_plan,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);
#ifdef __cplusplus
}
#endif

#endif
