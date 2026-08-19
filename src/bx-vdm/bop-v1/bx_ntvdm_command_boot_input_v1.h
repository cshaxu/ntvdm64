#ifndef BX_NTVDM_COMMAND_BOOT_INPUT_V1_H
#define BX_NTVDM_COMMAND_BOOT_INPUT_V1_H

#include <stdint.h>

#include "byob_profile.h"
#include "bx_ntvdm_readonly_namespace.h"

#define BX_NTVDM_COMMAND_BOOT_INPUT_V1_MAGIC 0x42584342u
#define BX_NTVDM_COMMAND_BOOT_INPUT_V1_VERSION 1u

/* COMMAND owns this copied description of its two boot-input names.  The
 * backing guest boot namespace remains a separate DEM/boot-namespace owner;
 * neither a BOP nor COMMAND's path producer borrows that mutable record. */
typedef struct bx_ntvdm_command_boot_input_v1 {
    uint32_t magic, abi_version, struct_bytes, drive_index;
    wchar_t config_path[BYOB_PROFILE_GUEST_PATH_MAX_CHARS];
    wchar_t autoexec_path[BYOB_PROFILE_GUEST_PATH_MAX_CHARS];
} bx_ntvdm_command_boot_input_v1;

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_command_boot_input_v1_initialize(
    bx_ntvdm_command_boot_input_v1 *value,
    const bx_ntvdm_readonly_namespace_v1 *namespace_value);
int bx_ntvdm_command_boot_input_v1_initialize_paths(
    bx_ntvdm_command_boot_input_v1 *value, uint32_t drive_index,
    const wchar_t *config_path, const wchar_t *autoexec_path);
int bx_ntvdm_command_boot_input_v1_valid(
    const bx_ntvdm_command_boot_input_v1 *value);
#ifdef __cplusplus
}
#endif

#endif
