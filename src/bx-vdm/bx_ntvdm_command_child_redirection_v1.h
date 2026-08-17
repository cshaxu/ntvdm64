#ifndef BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_H
#define BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_H

#include "bx_ntvdm_command_stream_session_v1.h"

#define BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_MAGIC 0x42584352u
#define BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_VERSION 1u

typedef struct bx_ntvdm_command_child_redirection_v1 {
    uint32_t magic, abi_version, struct_bytes, reserved0;
    uint32_t launch_count, completion_count, last_result, last_error;
} bx_ntvdm_command_child_redirection_v1;

void bx_ntvdm_command_child_redirection_v1_initialize(
    bx_ntvdm_command_child_redirection_v1 *value);
int bx_ntvdm_command_child_redirection_v1_valid(
    const bx_ntvdm_command_child_redirection_v1 *value);
/* Direct-only host-child seam.  Stream ownership stays in the caller-provided
 * COMMAND session; no token or HANDLE is returned to guest state. */
int bx_ntvdm_command_child_redirection_v1_launch(
    bx_ntvdm_command_child_redirection_v1 *value,
    const bx_ntvdm_command_stream_session_v1 *stream,
    const uint8_t *command, uint32_t command_bytes,
    const uint8_t *environment, uint32_t environment_bytes,
    const bx_ntvdm_command_host_context_v1 *host_context,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif