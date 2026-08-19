#include "bx_ntvdm_profile_search_snapshot_v1.h"

#include <string.h>

static int bx_ntvdm_profile_search_snapshot_v1_name(const wchar_t *path,
    wchar_t output[13])
{
    size_t index;
    if (path == 0 || output == 0 || path[0] != L'\\' || path[1] == L'\0') return 0;
    for (index = 0u; path[index + 1u] != L'\0'; ++index) {
        wchar_t character = path[index + 1u];
        if (index >= 12u || character == L'\\' || character > 0x7fu) return 0;
        output[index] = character;
    }
    output[index] = L'\0';
    return index != 0u;
}

static int bx_ntvdm_profile_search_snapshot_v1_set(
    bx_ntvdm_host_namespace_entry_v1 *entry,
    const bx_ntvdm_readonly_namespace_file_v1 *file,
    const byob_guest_dos_metadata_descriptor *metadata, int payload_required)
{
    if (entry == 0 || file == 0 || metadata == 0 ||
        (file->bytes == 0 && file->byte_count != 0u) ||
        (payload_required && (file->bytes == 0 || file->byte_count == 0u)) ||
        !bx_ntvdm_profile_search_snapshot_v1_name(file->path, entry->dos_name)) return 0;
    entry->attributes = metadata->attributes;
    entry->byte_count = file->byte_count;
    entry->dos_time = metadata->dos_time;
    entry->dos_date = metadata->dos_date;
    return 1;
}

static int bx_ntvdm_profile_search_snapshot_v1_content_valid(
    const bx_ntvdm_readonly_namespace_file_v1 *file, int payload_required)
{
    return file != 0 && !(file->bytes == 0 && file->byte_count != 0u) &&
        (!payload_required || (file->bytes != 0 && file->byte_count != 0u));
}

int bx_ntvdm_profile_search_snapshot_v1_initialize(
    bx_ntvdm_profile_search_snapshot_v1 *value,
    const bx_ntvdm_readonly_namespace_v1 *namespace_value,
    const byob_profile_selection *selection)
{
    if (value == 0 || namespace_value == 0 || selection == 0) return 0;
    memset(value, 0, sizeof(*value));
    if (
        !selection->has_guest_search_metadata || namespace_value->drive_index >= 26u ||
        (namespace_value->file_count != 4u && namespace_value->file_count != 5u) ||
        namespace_value->files[3].bytes == 0 || namespace_value->files[3].byte_count == 0u ||
        !bx_ntvdm_profile_search_snapshot_v1_set(&value->entries[0],
            &namespace_value->files[0], &selection->command_metadata, 1) ||
        !bx_ntvdm_profile_search_snapshot_v1_set(&value->entries[1],
            &namespace_value->files[1], &selection->config_metadata, 0) ||
        !bx_ntvdm_profile_search_snapshot_v1_set(&value->entries[2],
            &namespace_value->files[2], &selection->autoexec_metadata, 0) ||
        !bx_ntvdm_profile_search_snapshot_v1_set(&value->entries[3],
            &namespace_value->files[3], &selection->target_metadata, 1) ||
        (namespace_value->file_count == 5u &&
         !bx_ntvdm_profile_search_snapshot_v1_set(&value->entries[4],
             &namespace_value->files[4], &selection->terminal_quit_metadata, 1))) return 0;
    value->magic = BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_MAGIC;
    value->version = BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_VERSION;
    value->drive_index = namespace_value->drive_index;
    value->entry_count = namespace_value->file_count;
    return 1;
}

int bx_ntvdm_profile_search_snapshot_v1_reproject_contents(
    bx_ntvdm_profile_search_snapshot_v1 *value,
    const bx_ntvdm_readonly_namespace_v1 *namespace_value)
{
    bx_ntvdm_profile_search_snapshot_v1 updated;
    uint32_t index;
    if (!bx_ntvdm_profile_search_snapshot_v1_valid(value) || namespace_value == 0 ||
        namespace_value->drive_index != value->drive_index ||
        namespace_value->file_count != value->entry_count) return 0;
    updated = *value;
    for (index = 0u; index < value->entry_count; ++index) {
        wchar_t name[13];
        int payload_required = index == 0u || index >= 3u;
        if (!bx_ntvdm_profile_search_snapshot_v1_content_valid(
                &namespace_value->files[index], payload_required) ||
            !bx_ntvdm_profile_search_snapshot_v1_name(
                namespace_value->files[index].path, name) ||
            wcscmp(name, value->entries[index].dos_name) != 0) return 0;
        updated.entries[index].byte_count = namespace_value->files[index].byte_count;
    }
    *value = updated;
    return bx_ntvdm_profile_search_snapshot_v1_valid(value);
}

int bx_ntvdm_profile_search_snapshot_v1_valid(
    const bx_ntvdm_profile_search_snapshot_v1 *value)
{
    uint32_t index;
    if (value == 0 || value->magic != BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_MAGIC ||
        value->version != BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_VERSION ||
        value->drive_index >= 26u ||
        (value->entry_count != 4u && value->entry_count != 5u)) return 0;
    for (index = 0u; index < value->entry_count; ++index) {
        if (value->entries[index].dos_name[0] == L'\0') return 0;
    }
    return 1;
}
