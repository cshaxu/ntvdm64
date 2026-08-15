#include "bx_ntvdm_readonly_namespace.h"

#include <string.h>

#define BX_NTVDM_READONLY_NAMESPACE_TOKEN 0x4e534001u

static int bx_ntvdm_readonly_namespace_v1_copy_path(wchar_t *destination,
    size_t destination_count, const wchar_t *source)
{
    size_t length;
    if (destination == 0 || source == 0 || destination_count == 0u) return 0;
    length = wcslen(source);
    if (length >= destination_count) return 0;
    memcpy(destination, source, (length + 1u) * sizeof(*destination));
    return 1;
}

/* DEM's canonical relative pathname omits the leading backslash while the
 * boot declaration keeps it.  They denote the same root-relative DOS name. */
static int bx_ntvdm_readonly_namespace_v1_path_equal(const wchar_t *left,
    const wchar_t *right)
{
    if (left == 0 || right == 0) return 0;
    while (*left == L'\\' || *left == L'/') ++left;
    while (*right == L'\\' || *right == L'/') ++right;
    return _wcsicmp(left, right) == 0;
}

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
    value->files[0].dos_time = selection->command_metadata.dos_time;
    value->files[0].dos_date = selection->command_metadata.dos_date;
    if (!bx_ntvdm_readonly_namespace_v1_copy_path(value->files[0].path,
            sizeof(value->files[0].path) / sizeof(value->files[0].path[0]),
            selection->command_placement.path)) return 0;
    /* Paths and metadata are profile declarations only. The paired CONFIG /
       AUTOEXEC bytes are installed exclusively by the ready T204 provider. */
    value->files[1].dos_time = selection->config_metadata.dos_time;
    value->files[1].dos_date = selection->config_metadata.dos_date;
    if (!bx_ntvdm_readonly_namespace_v1_copy_path(value->files[1].path,
            sizeof(value->files[1].path) / sizeof(value->files[1].path[0]),
            selection->config_file.path)) return 0;
    value->files[2].dos_time = selection->autoexec_metadata.dos_time;
    value->files[2].dos_date = selection->autoexec_metadata.dos_date;
    if (!bx_ntvdm_readonly_namespace_v1_copy_path(value->files[2].path,
            sizeof(value->files[2].path) / sizeof(value->files[2].path[0]),
            selection->autoexec_file.path)) return 0;
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
    value->files[3].dos_time = selection->target_metadata.dos_time;
    value->files[3].dos_date = selection->target_metadata.dos_date;
    if (!bx_ntvdm_readonly_namespace_v1_copy_path(value->files[3].path,
            sizeof(value->files[3].path) / sizeof(value->files[3].path[0]), expected)) return 0;
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
    value->files[4].dos_time = selection->terminal_quit_metadata.dos_time;
    value->files[4].dos_date = selection->terminal_quit_metadata.dos_date;
    if (!bx_ntvdm_readonly_namespace_v1_copy_path(value->files[4].path,
            sizeof(value->files[4].path) / sizeof(value->files[4].path[0]), L"\\QUIT.COM")) return 0;
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
        if (bx_ntvdm_readonly_namespace_v1_path_equal(canonical_path,
                value->files[index].path)) {
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

int bx_ntvdm_readonly_namespace_v1_file_times(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t token,
    uint16_t *dos_time, uint16_t *dos_date)
{
    if (!value || !dos_time || !dos_date || !value->open ||
        token != value->generation || value->open_file_index >= value->file_count)
        return 0;
    *dos_time = value->files[value->open_file_index].dos_time;
    *dos_date = value->files[value->open_file_index].dos_date;
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_match_startup_path(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t drive_index,
    const wchar_t *canonical_path, uint64_t *byte_count_out)
{
    uint32_t index;
    if (byte_count_out != 0) *byte_count_out = 0u;
    if (value == 0 || canonical_path == 0 || drive_index != value->drive_index ||
        value->file_count < 3u) return 0;
    /* The namespace is the declared-image owner.  CONFIG/AUTOEXEC become
     * ready later than COMMAND/targets, but once a slot owns bytes it must
     * have the same immutable DEM visibility as every other declared image. */
    for (index = 0u; index < value->file_count; ++index) {
        if (value->files[index].bytes != 0 &&
            bx_ntvdm_readonly_namespace_v1_path_equal(canonical_path,
                value->files[index].path)) {
            if (byte_count_out != 0) *byte_count_out = value->files[index].byte_count;
            return 1;
        }
    }
    return 0;
}

int bx_ntvdm_readonly_namespace_v1_owns_token(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t token)
{
    return value != 0 && value->open != 0u && token == value->generation &&
        value->open_file_index < value->file_count;
}
