#include "bx_ntvdm_cmd_launch_envelope_v1.h"

#include <string.h>

#define BX_NTVDM_CMD_LAUNCH_ENVELOPE_V1_VERSION 1u

static int contained_command_path(const wchar_t path[BYOB_PROFILE_GUEST_PATH_MAX_CHARS])
{
    static const wchar_t expected[] = L"\\COMMAND.COM";
    uint32_t index;
    for (index = 0u; expected[index] != L'\0'; ++index) {
        if (index >= BYOB_PROFILE_GUEST_PATH_MAX_CHARS || path[index] != expected[index]) return 0;
    }
    return index < BYOB_PROFILE_GUEST_PATH_MAX_CHARS && path[index] == L'\0';
}

void bx_ntvdm_cmd_launch_envelope_v1_initialize(bx_ntvdm_cmd_launch_envelope_v1 *value)
{
    if (value == 0) return;
    memset(value, 0, sizeof(*value));
    value->version = BX_NTVDM_CMD_LAUNCH_ENVELOPE_V1_VERSION;
    value->struct_bytes = (uint32_t)sizeof(*value);
}

int bx_ntvdm_cmd_launch_envelope_v1_derive_contained_command(
    bx_ntvdm_cmd_launch_envelope_v1 *value,
    const bx_ntvdm_readonly_namespace_v1 *readonly_namespace)
{
    const wchar_t *source;
    uint32_t index;

    if (value == 0 || readonly_namespace == 0 || readonly_namespace->drive_index >= 26u ||
        readonly_namespace->files[0].bytes == 0 || readonly_namespace->files[0].byte_count == 0u ||
        !contained_command_path(readonly_namespace->files[0].path)) return 0;
    bx_ntvdm_cmd_launch_envelope_v1_initialize(value);
    source = readonly_namespace->files[0].path;
    value->executable_path[0] = (char)('A' + readonly_namespace->drive_index);
    value->executable_path[1] = ':';
    for (index = 0u; source[index] != L'\0'; ++index) {
        if (source[index] > 0x7f || index + 3u >= BX_NTVDM_CMD_LAUNCH_ENVELOPE_V1_PATH_BYTES) {
            bx_ntvdm_cmd_launch_envelope_v1_initialize(value);
            return 0;
        }
        value->executable_path[index + 2u] = (char)source[index];
    }
    value->executable_path[index + 2u] = '\0';
    value->drive_index = readonly_namespace->drive_index;
    value->extension = BX_NTVDM_CMD_LAUNCH_EXTENSION_V1_COM;
    value->executable_path_bytes = index + 2u;
    value->state = BX_NTVDM_CMD_LAUNCH_STATE_V1_PENDING;
    return 1;
}

int bx_ntvdm_cmd_launch_envelope_v1_take_once(
    bx_ntvdm_cmd_launch_envelope_v1 *value,
    bx_ntvdm_cmd_launch_envelope_v1 *out_value)
{
    if (value == 0 || out_value == 0 ||
        value->version != BX_NTVDM_CMD_LAUNCH_ENVELOPE_V1_VERSION ||
        value->struct_bytes != sizeof(*value) ||
        value->state != BX_NTVDM_CMD_LAUNCH_STATE_V1_PENDING ||
        value->drive_index >= 26u ||
        value->extension != BX_NTVDM_CMD_LAUNCH_EXTENSION_V1_COM ||
        value->executable_path_bytes < 3u ||
        value->executable_path_bytes >= BX_NTVDM_CMD_LAUNCH_ENVELOPE_V1_PATH_BYTES ||
        value->command_tail_bytes != 0u ||
        value->executable_path[value->executable_path_bytes] != '\0') return 0;
    memcpy(out_value, value, sizeof(*value));
    value->state = BX_NTVDM_CMD_LAUNCH_STATE_V1_DELIVERED;
    return 1;
}
