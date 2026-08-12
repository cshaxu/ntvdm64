#include "bx_ntvdm_readonly_namespace.h"

#include <string.h>

#define BX_NTVDM_READONLY_NAMESPACE_TOKEN 0x4e534001u

static const uint8_t bx_ntvdm_minimal_config_v1[] = "REM NTVDM64\r\n";
static const uint8_t bx_ntvdm_empty_autoexec_v1[] = "";

int bx_ntvdm_readonly_namespace_v1_initialize(
    bx_ntvdm_readonly_namespace_v1 *value, const byob_image *command,
    const byob_profile_selection *selection)
{
    if (value == 0 || command == 0 || selection == 0 || command->bytes == 0 ||
        command->byte_count == 0u || !selection->has_command_placement ||
        !selection->has_guest_boot_files || selection->command_placement.drive_index >= 26u ||
        wcscmp(selection->command_placement.path, L"\\COMMAND.COM") != 0 ||
        wcscmp(selection->config_file.path, L"\\CONFIG.SYS") != 0 ||
        selection->config_file.materialization != BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1 ||
        wcscmp(selection->autoexec_file.path, L"\\AUTOEXEC.BAT") != 0 ||
        selection->autoexec_file.materialization != BYOB_GUEST_BOOT_FILE_EMPTY_V1) return 0;
    memset(value, 0, sizeof(*value));
    value->files[0].bytes = command->bytes;
    value->files[0].byte_count = command->byte_count;
    wcscpy(value->files[0].path, selection->command_placement.path);
    value->files[1].bytes = bx_ntvdm_minimal_config_v1;
    value->files[1].byte_count = sizeof(bx_ntvdm_minimal_config_v1) - 1u;
    wcscpy(value->files[1].path, selection->config_file.path);
    value->files[2].bytes = bx_ntvdm_empty_autoexec_v1;
    value->files[2].byte_count = 0u;
    wcscpy(value->files[2].path, selection->autoexec_file.path);
    value->drive_index = selection->command_placement.drive_index;
    value->file_count = 3u;
    value->generation = BX_NTVDM_READONLY_NAMESPACE_TOKEN;
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_append_target(
    bx_ntvdm_readonly_namespace_v1 *value, const byob_image *target,
    const byob_profile_selection *selection)
{
    const wchar_t *expected;
    if (value == 0 || target == 0 || selection == 0 || value->open ||
        target->bytes == 0 || target->byte_count == 0u ||
        (!selection->has_target_placement && selection->declared_target_count != 2u) ||
        (selection->has_target_placement && selection->target_placement.drive_index != value->drive_index) ||
        (selection->declared_target_count == 2u &&
         selection->declared_targets[0].placement.drive_index != value->drive_index) ||
        (wcscmp(selection->target.file_name, L"TARGET.COM") != 0 &&
         wcscmp(selection->target.file_name, L"TARGET.EXE") != 0)) return 0;
    expected = wcscmp(selection->target.file_name, L"TARGET.COM") == 0 ?
        L"\\TARGET.COM" : L"\\TARGET.EXE";
    if ((selection->has_target_placement && wcscmp(selection->target_placement.path, expected) != 0) ||
        (selection->declared_target_count == 2u &&
         wcscmp(selection->declared_targets[0].placement.path, expected) != 0) ||
        value->files[3].path[0] != L'\0') return 0;
    value->files[3].bytes = target->bytes;
    value->files[3].byte_count = target->byte_count;
    wcscpy(value->files[3].path, expected);
    value->file_count = 4u;
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_append_terminal_quit(
    bx_ntvdm_readonly_namespace_v1 *value, const byob_image *terminal_quit,
    const byob_profile_selection *selection)
{
    if (!value || !terminal_quit || !selection || value->open || value->file_count != 4u ||
        selection->declared_target_count != 2u || !selection->declared_targets[1].terminal ||
        terminal_quit->bytes == 0 || terminal_quit->byte_count != 3u ||
        wcscmp(selection->declared_targets[1].component.file_name, L"QUIT.COM") != 0 ||
        wcscmp(selection->declared_targets[1].placement.path, L"\\QUIT.COM") != 0 ||
        selection->declared_targets[1].placement.drive_index != value->drive_index) return 0;
    value->files[4].bytes = terminal_quit->bytes;
    value->files[4].byte_count = terminal_quit->byte_count;
    wcscpy(value->files[4].path, L"\\QUIT.COM");
    value->file_count = 5u;
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_open(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t drive_index,
    const wchar_t *canonical_path, uint32_t *token, uint64_t *byte_count)
{
    uint32_t index;
    if (value == 0 || token == 0 || byte_count == 0 || canonical_path == 0 || value->open ||
        drive_index != value->drive_index) return 0;
    for (index = 0u; index < value->file_count; ++index) {
        if (wcscmp(canonical_path, value->files[index].path) == 0) {
            value->open = 1u;
            value->open_file_index = index;
            value->offset = 0u;
            *token = value->generation;
            *byte_count = value->files[index].byte_count;
            return 1;
        }
    }
    return 0;
}

int bx_ntvdm_readonly_namespace_v1_seek(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t token, uint64_t offset)
{
    if (value == 0 || !value->open || token != value->generation ||
        offset > value->files[value->open_file_index].byte_count) return 0;
    value->offset = offset;
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_read(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t token, uint8_t *destination,
    uint32_t requested_bytes, uint32_t *read_bytes)
{
    uint64_t remaining;
    uint32_t count;
    if (read_bytes != 0) *read_bytes = 0u;
    if (value == 0 || !value->open || token != value->generation || destination == 0 ||
        read_bytes == 0 || requested_bytes > BX_NTVDM_READONLY_NAMESPACE_MAX_READ_BYTES) return 0;
    remaining = value->files[value->open_file_index].byte_count - value->offset;
    count = remaining < requested_bytes ? (uint32_t)remaining : requested_bytes;
    if (count != 0u) memcpy(destination,
        value->files[value->open_file_index].bytes + value->offset, count);
    value->offset += count;
    *read_bytes = count;
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_close(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t token)
{
    if (value == 0 || !value->open || token != value->generation) return 0;
    value->open = 0u;
    value->offset = 0u;
    ++value->generation;
    if (value->generation == 0u) ++value->generation;
    return 1;
}
