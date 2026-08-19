#include "bx_ntvdm_readonly_namespace.h"

#include <string.h>

#define BX_NTVDM_READONLY_NAMESPACE_TOKEN UINT32_C(0x4e530000)
#define BX_NTVDM_READONLY_NAMESPACE_SLOT_MASK UINT32_C(0x000000ff)

static int copy_path(wchar_t *destination, size_t destination_count, const wchar_t *source)
{
    size_t length;
    if (destination == 0 || source == 0 || destination_count == 0u) return 0;
    length = wcslen(source);
    if (length >= destination_count) return 0;
    memcpy(destination, source, (length + 1u) * sizeof(*destination));
    return 1;
}

static int path_equal(const wchar_t *left, const wchar_t *right)
{
    if (left == 0 || right == 0) return 0;
    while (*left == L'\\' || *left == L'/') ++left;
    while (*right == L'\\' || *right == L'/') ++right;
    return _wcsicmp(left, right) == 0;
}

static uint32_t token_for(uint32_t index, uint32_t generation)
{ return (generation << 8u) | (index + 1u); }

static int decode(uint32_t token, uint32_t *index_out, uint32_t *generation_out)
{
    uint32_t index;
    if (token == 0u || index_out == 0 || generation_out == 0) return 0;
    index = (token & BX_NTVDM_READONLY_NAMESPACE_SLOT_MASK) - 1u;
    if (index >= BX_NTVDM_READONLY_NAMESPACE_MAX_OPEN_HANDLES) return 0;
    *index_out = index;
    *generation_out = token >> 8u;
    return *generation_out != 0u;
}

static bx_ntvdm_readonly_namespace_handle_v1 *handle_mutable(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t token)
{
    uint32_t index, generation;
    if (value == 0 || !decode(token, &index, &generation) ||
        value->handles[index].in_use == 0u ||
        value->handles[index].generation != generation ||
        value->handles[index].file_index >= value->file_count) return 0;
    return &value->handles[index];
}

static const bx_ntvdm_readonly_namespace_handle_v1 *handle_const(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t token)
{
    uint32_t index, generation;
    if (value == 0 || !decode(token, &index, &generation) ||
        value->handles[index].in_use == 0u ||
        value->handles[index].generation != generation ||
        value->handles[index].file_index >= value->file_count) return 0;
    return &value->handles[index];
}

static void summary(bx_ntvdm_readonly_namespace_v1 *value,
    const bx_ntvdm_readonly_namespace_handle_v1 *handle)
{
    if (value == 0) return;
    if (handle == 0) {
        value->open_file_index = UINT32_MAX;
        value->offset = 0u;
        return;
    }
    value->open_file_index = handle->file_index;
    value->offset = handle->offset;
}

int bx_ntvdm_readonly_namespace_v1_initialize(
    bx_ntvdm_readonly_namespace_v1 *value, const byob_image *command,
    const byob_profile_selection *selection)
{
    uint32_t index;
    if (value == 0 || command == 0 || selection == 0 || command->bytes == 0 ||
        command->byte_count == 0u || !selection->has_command_placement ||
        !selection->has_guest_boot_files || selection->command_placement.drive_index >= 26u ||
        wcscmp(selection->command_placement.path, L"\\COMMAND.COM") != 0 ||
        wcscmp(selection->config_file.path, L"\\CONFIG.SYS") != 0 ||
        selection->config_file.materialization != BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1 ||
        wcscmp(selection->autoexec_file.path, L"\\AUTOEXEC.BAT") != 0 ||
        selection->autoexec_file.materialization != BYOB_GUEST_BOOT_FILE_EMPTY_V1) return 0;
    memset(value, 0, sizeof(*value));
    for (index = 0u; index < BX_NTVDM_READONLY_NAMESPACE_MAX_OPEN_HANDLES; ++index)
        value->handles[index].generation = UINT32_C(0x004e53);
    value->files[0].bytes = command->bytes;
    value->files[0].byte_count = command->byte_count;
    value->files[0].dos_time = selection->command_metadata.dos_time;
    value->files[0].dos_date = selection->command_metadata.dos_date;
    if (!copy_path(value->files[0].path,
            sizeof(value->files[0].path) / sizeof(value->files[0].path[0]),
            selection->command_placement.path)) return 0;
    value->files[1].dos_time = selection->config_metadata.dos_time;
    value->files[1].dos_date = selection->config_metadata.dos_date;
    if (!copy_path(value->files[1].path,
            sizeof(value->files[1].path) / sizeof(value->files[1].path[0]),
            selection->config_file.path)) return 0;
    value->files[2].dos_time = selection->autoexec_metadata.dos_time;
    value->files[2].dos_date = selection->autoexec_metadata.dos_date;
    if (!copy_path(value->files[2].path,
            sizeof(value->files[2].path) / sizeof(value->files[2].path[0]),
            selection->autoexec_file.path)) return 0;
    value->drive_index = selection->command_placement.drive_index;
    value->file_count = 3u;
    value->generation = BX_NTVDM_READONLY_NAMESPACE_TOKEN;
    summary(value, 0);
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_append_target(
    bx_ntvdm_readonly_namespace_v1 *value, const byob_image *target,
    const byob_profile_selection *selection)
{
    const wchar_t *placement;
    if (value == 0 || target == 0 || selection == 0 || value->open != 0u ||
        target->bytes == 0 || target->byte_count == 0u ||
        selection->declared_target_count == 0u || selection->declared_target_count > 2u ||
        selection->declared_targets[0].placement.drive_index != value->drive_index ||
        selection->declared_targets[0].placement.path[0] != L'\\' ||
        wcscmp(selection->declared_targets[0].placement.path + 1,
            selection->target.file_name) != 0 || value->files[3].path[0] != L'\0') return 0;
    placement = selection->declared_targets[0].placement.path;
    value->files[3].bytes = target->bytes;
    value->files[3].byte_count = target->byte_count;
    value->files[3].dos_time = selection->target_metadata.dos_time;
    value->files[3].dos_date = selection->target_metadata.dos_date;
    if (!copy_path(value->files[3].path,
            sizeof(value->files[3].path) / sizeof(value->files[3].path[0]), placement)) return 0;
    value->file_count = 4u;
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_append_terminal_quit(
    bx_ntvdm_readonly_namespace_v1 *value, const byob_image *terminal_quit,
    const byob_profile_selection *selection)
{
    if (!value || !terminal_quit || !selection || value->open != 0u || value->file_count != 4u ||
        selection->declared_target_count != 2u || !selection->declared_targets[1].terminal ||
        terminal_quit->bytes == 0 || terminal_quit->byte_count != 3u ||
        wcscmp(selection->declared_targets[1].component.file_name, L"QUIT.COM") != 0 ||
        wcscmp(selection->declared_targets[1].placement.path, L"\\QUIT.COM") != 0 ||
        selection->declared_targets[1].placement.drive_index != value->drive_index) return 0;
    value->files[4].bytes = terminal_quit->bytes;
    value->files[4].byte_count = terminal_quit->byte_count;
    value->files[4].dos_time = selection->terminal_quit_metadata.dos_time;
    value->files[4].dos_date = selection->terminal_quit_metadata.dos_date;
    if (!copy_path(value->files[4].path,
            sizeof(value->files[4].path) / sizeof(value->files[4].path[0]), L"\\QUIT.COM")) return 0;
    value->file_count = 5u;
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_open(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t drive_index,
    const wchar_t *canonical_path, uint32_t *token, uint64_t *byte_count)
{
    uint32_t file_index, handle_index;
    if (value == 0 || token == 0 || byte_count == 0 || canonical_path == 0 ||
        drive_index != value->drive_index) return 0;
    for (file_index = 0u; file_index < value->file_count; ++file_index) {
        if (!path_equal(canonical_path, value->files[file_index].path)) continue;
        for (handle_index = 0u; handle_index < BX_NTVDM_READONLY_NAMESPACE_MAX_OPEN_HANDLES;
                ++handle_index) {
            bx_ntvdm_readonly_namespace_handle_v1 *handle = &value->handles[handle_index];
            if (handle->in_use != 0u) continue;
            handle->in_use = 1u;
            handle->file_index = file_index;
            handle->offset = 0u;
            *token = token_for(handle_index, handle->generation);
            *byte_count = value->files[file_index].byte_count;
            ++value->open;
            summary(value, handle);
            return 1;
        }
        return 0;
    }
    return 0;
}

int bx_ntvdm_readonly_namespace_v1_seek(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t token, uint64_t offset)
{
    bx_ntvdm_readonly_namespace_handle_v1 *handle = handle_mutable(value, token);
    if (handle == 0 || offset > value->files[handle->file_index].byte_count) return 0;
    handle->offset = offset;
    summary(value, handle);
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_read(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t token, uint8_t *destination,
    uint32_t requested_bytes, uint32_t *read_bytes)
{
    bx_ntvdm_readonly_namespace_handle_v1 *handle;
    uint64_t remaining;
    uint32_t count;
    if (read_bytes != 0) *read_bytes = 0u;
    handle = handle_mutable(value, token);
    if (handle == 0 || destination == 0 || read_bytes == 0 ||
        requested_bytes > BX_NTVDM_READONLY_NAMESPACE_MAX_READ_BYTES) return 0;
    remaining = value->files[handle->file_index].byte_count - handle->offset;
    count = remaining < requested_bytes ? (uint32_t)remaining : requested_bytes;
    if (count != 0u) memcpy(destination,
        value->files[handle->file_index].bytes + handle->offset, count);
    handle->offset += count;
    *read_bytes = count;
    summary(value, handle);
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_close(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t token)
{
    bx_ntvdm_readonly_namespace_handle_v1 *handle = handle_mutable(value, token);
    if (handle == 0 || value->open == 0u) return 0;
    handle->in_use = 0u;
    handle->file_index = 0u;
    handle->offset = 0u;
    ++handle->generation;
    if (handle->generation == 0u) handle->generation = 1u;
    --value->open;
    ++value->generation;
    if (value->generation == 0u) ++value->generation;
    if (value->open != 0u) {
        uint32_t index;
        for (index = 0u; index < BX_NTVDM_READONLY_NAMESPACE_MAX_OPEN_HANDLES; ++index) {
            if (value->handles[index].in_use != 0u) {
                summary(value, &value->handles[index]);
                break;
            }
        }
    } else {
        summary(value, 0);
    }
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_position(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t token,
    uint64_t *offset_out, uint64_t *size_out)
{
    const bx_ntvdm_readonly_namespace_handle_v1 *handle = handle_const(value, token);
    if (offset_out != 0) *offset_out = 0u;
    if (size_out != 0) *size_out = 0u;
    if (handle == 0 || offset_out == 0 || size_out == 0) return 0;
    *offset_out = handle->offset;
    *size_out = value->files[handle->file_index].byte_count;
    return 1;
}

int bx_ntvdm_readonly_namespace_v1_file_times(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t token,
    uint16_t *dos_time, uint16_t *dos_date)
{
    const bx_ntvdm_readonly_namespace_handle_v1 *handle = handle_const(value, token);
    if (!dos_time || !dos_date || handle == 0) return 0;
    *dos_time = value->files[handle->file_index].dos_time;
    *dos_date = value->files[handle->file_index].dos_date;
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
    for (index = 0u; index < value->file_count; ++index) {
        if (value->files[index].bytes != 0 &&
            path_equal(canonical_path, value->files[index].path)) {
            if (byte_count_out != 0) *byte_count_out = value->files[index].byte_count;
            return 1;
        }
    }
    return 0;
}

int bx_ntvdm_readonly_namespace_v1_query_startup_file(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t drive_index,
    const wchar_t *canonical_path, uint64_t *byte_count_out,
    uint16_t *dos_time_out, uint16_t *dos_date_out)
{
    uint32_t index;
    if (byte_count_out != 0) *byte_count_out = 0u;
    if (dos_time_out != 0) *dos_time_out = 0u;
    if (dos_date_out != 0) *dos_date_out = 0u;
    if (value == 0 || canonical_path == 0 || byte_count_out == 0 ||
        dos_time_out == 0 || dos_date_out == 0 || drive_index != value->drive_index ||
        value->file_count < 3u) return 0;
    for (index = 0u; index < value->file_count; ++index) {
        if (value->files[index].bytes != 0 &&
            path_equal(canonical_path, value->files[index].path)) {
            *byte_count_out = value->files[index].byte_count;
            *dos_time_out = value->files[index].dos_time;
            *dos_date_out = value->files[index].dos_date;
            return 1;
        }
    }
    return 0;
}

uint32_t bx_ntvdm_readonly_namespace_v1_declared_slot(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t drive_index,
    const wchar_t *canonical_path, uint32_t *bytes_ready_out)
{
    uint32_t index;
    if (bytes_ready_out != 0) *bytes_ready_out = 0u;
    if (value == 0 || canonical_path == 0 || drive_index != value->drive_index ||
        value->file_count < 3u) return 0u;
    for (index = 0u; index < value->file_count; ++index) {
        if (path_equal(canonical_path, value->files[index].path)) {
            if (bytes_ready_out != 0)
                *bytes_ready_out = value->files[index].bytes != 0 ? 1u : 0u;
            return index + 1u;
        }
    }
    return 0u;
}

int bx_ntvdm_readonly_namespace_v1_owns_token(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t token)
{ return handle_const(value, token) != 0; }