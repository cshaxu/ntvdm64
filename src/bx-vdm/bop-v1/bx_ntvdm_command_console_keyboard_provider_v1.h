#ifndef BX_NTVDM_COMMAND_CONSOLE_KEYBOARD_PROVIDER_V1_H
#define BX_NTVDM_COMMAND_CONSOLE_KEYBOARD_PROVIDER_V1_H

#include "bx_ntvdm_cmd_keyboard_layout_service.h"
#include "bx_ntvdm_command_console_capability_v1.h"

#define BX_NTVDM_COMMAND_CONSOLE_KEYBOARD_PROVIDER_V1_MAGIC 0x4258434bu
#define BX_NTVDM_COMMAND_CONSOLE_KEYBOARD_PROVIDER_V1_VERSION 1u

/* COMMAND owns console and keyboard-layout policy as one historical
 * component.  The embedded console state is not a generic adapter or Bochs
 * facility; it records the source-shaped no-VDD/no-install disposition. */
typedef struct bx_ntvdm_command_console_keyboard_provider_v1 {
    uint32_t magic, abi_version, struct_bytes, initialized;
    bx_ntvdm_command_console_capability_v1 console;
} bx_ntvdm_command_console_keyboard_provider_v1;

int bx_ntvdm_command_console_keyboard_provider_v1_initialize(
    bx_ntvdm_command_console_keyboard_provider_v1 *provider);
int bx_ntvdm_command_console_keyboard_provider_v1_valid(
    const bx_ntvdm_command_console_keyboard_provider_v1 *provider);
int bx_ntvdm_command_console_keyboard_provider_v1_owns_service(uint8_t service);
int bx_ntvdm_command_console_keyboard_provider_v1_dispatch(
    bx_ntvdm_command_console_keyboard_provider_v1 *provider,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
