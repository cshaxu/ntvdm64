#include "bx_ntvdm_command_boot_input_v1.h"

#include <string.h>

static int copy_guest_path(wchar_t *destination, const wchar_t *source)
{
    size_t length = 0u;
    if (destination == 0 || source == 0 || source[0] != L'\\') return 0;
    while (length < BYOB_PROFILE_GUEST_PATH_MAX_CHARS && source[length] != L'\0')
        ++length;
    if (length == 0u || length == BYOB_PROFILE_GUEST_PATH_MAX_CHARS) return 0;
    memcpy(destination, source, (length + 1u) * sizeof(*destination));
    return 1;
}

int bx_ntvdm_command_boot_input_v1_valid(
    const bx_ntvdm_command_boot_input_v1 *value)
{
    wchar_t config_copy[BYOB_PROFILE_GUEST_PATH_MAX_CHARS];
    wchar_t autoexec_copy[BYOB_PROFILE_GUEST_PATH_MAX_CHARS];
    return value != 0 && value->magic == BX_NTVDM_COMMAND_BOOT_INPUT_V1_MAGIC &&
        value->abi_version == BX_NTVDM_COMMAND_BOOT_INPUT_V1_VERSION &&
        value->struct_bytes == sizeof(*value) && value->drive_index < 26u &&
        copy_guest_path(config_copy, value->config_path) &&
        copy_guest_path(autoexec_copy, value->autoexec_path);
}

int bx_ntvdm_command_boot_input_v1_initialize(
    bx_ntvdm_command_boot_input_v1 *value,
    const bx_ntvdm_readonly_namespace_v1 *namespace_value)
{
    if (value == 0 || namespace_value == 0 || namespace_value->drive_index >= 26u ||
        namespace_value->file_count < 3u) return 0;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_COMMAND_BOOT_INPUT_V1_MAGIC;
    value->abi_version = BX_NTVDM_COMMAND_BOOT_INPUT_V1_VERSION;
    value->struct_bytes = sizeof(*value);
    value->drive_index = namespace_value->drive_index;
    if (!copy_guest_path(value->config_path, namespace_value->files[1].path) ||
        !copy_guest_path(value->autoexec_path, namespace_value->files[2].path)) {
        memset(value, 0, sizeof(*value));
        return 0;
    }
    return bx_ntvdm_command_boot_input_v1_valid(value);
}
