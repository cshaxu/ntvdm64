#ifndef BX_NTVDM_COMMAND_STREAM_SESSION_V1_H
#define BX_NTVDM_COMMAND_STREAM_SESSION_V1_H

#include "bx_ntvdm_cmd_get_next_service.h"
#include "bx_ntvdm_command_host_context_v1.h"

#define BX_NTVDM_COMMAND_STREAM_SESSION_V1_MAGIC 0x42584354u
#define BX_NTVDM_COMMAND_STREAM_SESSION_V1_VERSION 1u
#define BX_NTVDM_COMMAND_STREAM_SESSION_V1_SLOT_COUNT 3u

/* This is COMMAND-private state, not bridge data.  Captured values are never
 * copied to a CPU result, guest range, or another component. */
typedef struct bx_ntvdm_command_stream_session_v1 {
    uint32_t magic, abi_version, struct_bytes, generation;
    uint32_t available_mask, validated_record_count, rejected_record_count,
        reserved0;
    uint32_t launch_count, completion_count, last_result, last_error;
    uintptr_t private_handle_values[BX_NTVDM_COMMAND_STREAM_SESSION_V1_SLOT_COUNT];
} bx_ntvdm_command_stream_session_v1;

#ifdef __cplusplus
extern "C" {
#endif
/* Initialization is host-neutral.  Only the admitted CLI composition may
 * capture the three standard handles, and only after Direct selection. */
void bx_ntvdm_command_stream_session_v1_initialize(
    bx_ntvdm_command_stream_session_v1 *value);
int bx_ntvdm_command_stream_session_v1_admit_cli_standard_handles(
    bx_ntvdm_command_stream_session_v1 *value);
int bx_ntvdm_command_stream_session_v1_valid(
    const bx_ntvdm_command_stream_session_v1 *value);
int bx_ntvdm_command_stream_session_v1_dispatch_stream(
    const bx_ntvdm_command_stream_session_v1 *value, int direct_granted,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_command_stream_session_v1_validate_std_handles(
    bx_ntvdm_command_stream_session_v1 *value, const uint8_t *payload,
    uint32_t payload_bytes);
int bx_ntvdm_command_stream_session_v1_launch(
    bx_ntvdm_command_stream_session_v1 *value, const uint8_t *command,
    uint32_t command_bytes, const uint8_t *environment,
    uint32_t environment_bytes,
    const bx_ntvdm_command_host_context_v1 *host_context,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);
#ifdef __cplusplus
}
#endif

#endif
