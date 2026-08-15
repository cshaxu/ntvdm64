#ifndef BX_NTVDM_COMMAND_LAUNCH_EXECUTION_PROVIDER_V1_H
#define BX_NTVDM_COMMAND_LAUNCH_EXECUTION_PROVIDER_V1_H

/* COMMAND's 54:06/07/08/0A component boundary.  It owns private stream
 * capability state and execution disposition; callers never select one leaf
 * directly. */
#include "bx_ntvdm_command_execution_lifecycle_v1.h"
#include "bx_ntvdm_command_stream_child_v1.h"

#define BX_NTVDM_COMMAND_LAUNCH_EXECUTION_PROVIDER_V1_MAGIC UINT32_C(0x4258434c)
#define BX_NTVDM_COMMAND_LAUNCH_EXECUTION_PROVIDER_V1_VERSION UINT32_C(1)

typedef struct bx_ntvdm_command_launch_execution_provider_v1 {
    uint32_t magic, abi_version, struct_bytes, initialized;
    bx_ntvdm_command_execution_lifecycle_v1 lifecycle;
    bx_ntvdm_command_stream_child_v1 stream_child;
} bx_ntvdm_command_launch_execution_provider_v1;

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_command_launch_execution_provider_v1_initialize(
    bx_ntvdm_command_launch_execution_provider_v1 *value);
int bx_ntvdm_command_launch_execution_provider_v1_valid(
    const bx_ntvdm_command_launch_execution_provider_v1 *value);
int bx_ntvdm_command_launch_execution_provider_v1_owns_service(uint8_t service);
#ifdef __cplusplus
}
#endif
#endif
