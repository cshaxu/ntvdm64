#ifndef BX_NTVDM_COMMAND_STREAM_CHILD_V1_H
#define BX_NTVDM_COMMAND_STREAM_CHILD_V1_H

#include "bx_ntvdm_cmd_get_next_service.h"

#define BX_NTVDM_COMMAND_STREAM_CHILD_V1_MAGIC 0x42584354u
#define BX_NTVDM_COMMAND_STREAM_CHILD_V1_VERSION 1u
#define BX_NTVDM_COMMAND_STREAM_CHILD_V1_SLOT_COUNT 3u

/* This is COMMAND-private state, not bridge data.  Captured values are never
 * copied to a CPU result, guest range, or another component. */
typedef struct bx_ntvdm_command_stream_child_v1 {
    uint32_t magic, abi_version, struct_bytes, generation;
    uint32_t available_mask, reserved0;
    uintptr_t private_handle_values[BX_NTVDM_COMMAND_STREAM_CHILD_V1_SLOT_COUNT];
} bx_ntvdm_command_stream_child_v1;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_command_stream_child_v1_initialize(
    bx_ntvdm_command_stream_child_v1 *value);
int bx_ntvdm_command_stream_child_v1_valid(
    const bx_ntvdm_command_stream_child_v1 *value);
int bx_ntvdm_command_stream_child_v1_dispatch_stream(
    const bx_ntvdm_command_stream_child_v1 *value, int direct_granted,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);
#ifdef __cplusplus
}
#endif

#endif
