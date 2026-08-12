#include "bx_ntvdm_cmd_launch_envelope_v1.h"

#include <stdio.h>
#include <string.h>

static void contained_namespace(bx_ntvdm_readonly_namespace_v1 *value, uint8_t *bytes)
{
    memset(value, 0, sizeof(*value));
    value->files[0].bytes = bytes;
    value->files[0].byte_count = 1u;
    wcscpy(value->files[0].path, L"\\COMMAND.COM");
    value->drive_index = 2u;
}

int main(void)
{
    uint8_t bytes[] = { 0x90u };
    bx_ntvdm_readonly_namespace_v1 source;
    bx_ntvdm_cmd_launch_envelope_v1 value, delivered;

    contained_namespace(&source, bytes);
    bx_ntvdm_cmd_launch_envelope_v1_initialize(&value);
    if (value.version != 1u || value.struct_bytes != sizeof(value) ||
        value.state != BX_NTVDM_CMD_LAUNCH_STATE_V1_EMPTY ||
        !bx_ntvdm_cmd_launch_envelope_v1_derive_contained_command(&value, &source) ||
        value.state != BX_NTVDM_CMD_LAUNCH_STATE_V1_PENDING || value.drive_index != 2u ||
        value.extension != BX_NTVDM_CMD_LAUNCH_EXTENSION_V1_COM ||
        value.executable_path_bytes != 14u || strcmp(value.executable_path, "C:\\COMMAND.COM") != 0 ||
        value.command_tail_bytes != 0u ||
        !bx_ntvdm_cmd_launch_envelope_v1_take_once(&value, &delivered) ||
        strcmp(delivered.executable_path, "C:\\COMMAND.COM") != 0 ||
        delivered.state != BX_NTVDM_CMD_LAUNCH_STATE_V1_PENDING ||
        value.state != BX_NTVDM_CMD_LAUNCH_STATE_V1_DELIVERED ||
        bx_ntvdm_cmd_launch_envelope_v1_take_once(&value, &delivered)) return 1;

    wcscpy(source.files[0].path, L"\\OTHER.COM");
    if (bx_ntvdm_cmd_launch_envelope_v1_derive_contained_command(&value, &source)) return 1;
    wcscpy(source.files[0].path, L"\\COMMAND.COM");
    source.files[0].path[1] = 0x80;
    if (bx_ntvdm_cmd_launch_envelope_v1_derive_contained_command(&value, &source)) return 1;
    wmemset(source.files[0].path, L'X', BYOB_PROFILE_GUEST_PATH_MAX_CHARS);
    if (bx_ntvdm_cmd_launch_envelope_v1_derive_contained_command(&value, &source)) return 1;
    puts("bx-ntvdm-cmd-launch-envelope-v1-test: contained one-shot command contract verified");
    return 0;
}
