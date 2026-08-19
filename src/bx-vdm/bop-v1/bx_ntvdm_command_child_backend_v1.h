#ifndef BX_NTVDM_COMMAND_CHILD_BACKEND_V1_H
#define BX_NTVDM_COMMAND_CHILD_BACKEND_V1_H

#include "bx_ntvdm_command_stream_session_v1.h"

#define BX_NTVDM_COMMAND_CHILD_BACKEND_V1_MAGIC UINT32_C(0x42584342)
#define BX_NTVDM_COMMAND_CHILD_BACKEND_V1_VERSION UINT32_C(1)
#define BX_NTVDM_COMMAND_CHILD_BACKEND_V1_COMMAND_BYTES 260u

/* This is strictly COMMAND-private provider data.  It is not a Bochs bridge,
 * guest ABI or CLI surface.  The stream pointer identifies an already-admitted
 * session; its host handles never leave the backend. */
typedef struct bx_ntvdm_command_child_backend_v1_request {
    uint32_t magic, abi_version, struct_bytes, launch_service;
    const uint8_t *command; uint32_t command_bytes;
    const uint8_t *environment; uint32_t environment_bytes;
    const bx_ntvdm_command_host_context_v1 *host_context;
    const bx_ntvdm_command_stream_session_v1 *stream;
} bx_ntvdm_command_child_backend_v1_request;

typedef struct bx_ntvdm_command_child_backend_v1_result {
    uint32_t magic, abi_version, struct_bytes, completed;
    uint32_t exit_code, host_error, reserved0, reserved1;
} bx_ntvdm_command_child_backend_v1_result;

typedef int (*bx_ntvdm_command_child_backend_v1_execute)(
    const bx_ntvdm_command_child_backend_v1_request *request,
    bx_ntvdm_command_child_backend_v1_result *result);

void bx_ntvdm_command_child_backend_v1_result_clear(
    bx_ntvdm_command_child_backend_v1_result *result);
int bx_ntvdm_command_child_backend_v1_request_valid(
    const bx_ntvdm_command_child_backend_v1_request *request);
int bx_ntvdm_command_child_backend_v1_result_valid(
    const bx_ntvdm_command_child_backend_v1_result *result);
/* Direct profile only.  All HANDLE ownership begins and ends in this unit. */
int bx_ntvdm_command_child_backend_v1_execute_direct(
    const bx_ntvdm_command_child_backend_v1_request *request,
    bx_ntvdm_command_child_backend_v1_result *result);

#endif