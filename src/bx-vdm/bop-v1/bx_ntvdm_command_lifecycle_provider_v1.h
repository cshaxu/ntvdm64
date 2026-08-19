#ifndef BX_NTVDM_COMMAND_LIFECYCLE_PROVIDER_V1_H
#define BX_NTVDM_COMMAND_LIFECYCLE_PROVIDER_V1_H

#include "bx_ntvdm_cmd_get_next_service.h"

#define BX_NTVDM_COMMAND_LIFECYCLE_PROVIDER_V1_MAGIC UINT32_C(0x42584359)
#define BX_NTVDM_COMMAND_LIFECYCLE_PROVIDER_V1_VERSION UINT32_C(1)
#define BX_NTVDM_COMMAND_TERMINAL_V1_MAGIC UINT32_C(0x42584354)
#define BX_NTVDM_COMMAND_TERMINAL_V1_VERSION UINT32_C(1)

enum bx_ntvdm_command_terminal_kind_v1 {
    BX_NTVDM_COMMAND_TERMINAL_V1_NONE = 0u,
    BX_NTVDM_COMMAND_TERMINAL_V1_TOP_LEVEL_EXIT
};

/* Adapter-local COMMAND lifecycle fact.  It contains no guest location,
 * selector, provider pointer, host handle, or fabricated DOS exit code. */
typedef struct bx_ntvdm_command_terminal_v1 {
    uint32_t magic, abi_version, struct_bytes, present;
    uint32_t terminal_kind, has_dos_exit_code, dos_exit_code, reserved0;
} bx_ntvdm_command_terminal_v1;

/* Original COMMAND lifecycle component: cmdExitVDM, cmdSaveWorld and
 * cmdReturnExitCode.  It has no engine/CLI result ownership. */
typedef struct bx_ntvdm_command_lifecycle_provider_v1 {
    uint32_t magic, abi_version, struct_bytes, initialized;
    bx_ntvdm_command_terminal_v1 terminal;
} bx_ntvdm_command_lifecycle_provider_v1;

int bx_ntvdm_command_lifecycle_provider_v1_initialize(
    bx_ntvdm_command_lifecycle_provider_v1 *provider);
int bx_ntvdm_command_lifecycle_provider_v1_valid(
    const bx_ntvdm_command_lifecycle_provider_v1 *provider);
int bx_ntvdm_command_lifecycle_provider_v1_owns_service(uint8_t service);
int bx_ntvdm_command_lifecycle_provider_v1_copy_terminal(
    const bx_ntvdm_command_lifecycle_provider_v1 *provider,
    bx_ntvdm_command_terminal_v1 *terminal);
int bx_ntvdm_command_lifecycle_provider_v1_dispatch(
    bx_ntvdm_command_lifecycle_provider_v1 *provider,
    bx_ntvdm_cmd_get_next_state_v1 *get_next,
    const byob_launch_plan_v2 *launch_plan,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
