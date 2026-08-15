#ifndef BX_NTVDM_COMMAND_HOST_CONTEXT_V1_H
#define BX_NTVDM_COMMAND_HOST_CONTEXT_V1_H

#include <stdint.h>

#define BX_NTVDM_COMMAND_HOST_CONTEXT_V1_MAGIC 0x42584348u
#define BX_NTVDM_COMMAND_HOST_CONTEXT_V1_VERSION 1u
/* OpenNT's MAXIMUM_VDM_CURRENT_DIR (64) plus drive, colon, slash and NUL. */
#define BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES 68u
/* The shared checked write ABI presently carries at most 4096 bytes.  A
 * COMSPEC line can consume at most 73 bytes, leaving this snapshot budget. */
#define BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES 4023u
#define BX_NTVDM_COMMAND_HOST_CONTEXT_V1_PROCESSOR_BYTES 260u

/* This is copied at CLI admission.  It contains no host handle or pathname
 * pointer, and a BOP provider must never recapture process state through it. */
typedef struct bx_ntvdm_command_host_context_v1 {
    uint32_t magic, abi_version, struct_bytes, selected_drive;
    uint32_t directory_bytes, environment_bytes, processor_bytes;
    uint8_t selected_directory[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES];
    uint8_t environment[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES];
    uint8_t processor[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_PROCESSOR_BYTES];
} bx_ntvdm_command_host_context_v1;

int bx_ntvdm_command_host_context_v1_initialize(
    bx_ntvdm_command_host_context_v1 *context, uint32_t selected_drive,
    const uint8_t *directory, uint32_t directory_bytes);
int bx_ntvdm_command_host_context_v1_set_environment(
    bx_ntvdm_command_host_context_v1 *context, const uint8_t *environment,
    uint32_t environment_bytes);
int bx_ntvdm_command_host_context_v1_set_processor(
    bx_ntvdm_command_host_context_v1 *context, const uint8_t *processor,
    uint32_t processor_bytes);
int bx_ntvdm_command_host_context_v1_valid(
    const bx_ntvdm_command_host_context_v1 *context);

#endif
