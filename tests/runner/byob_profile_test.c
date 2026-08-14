#include "byob_profile.h"
#include "byob_image.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>

static const char sha256_abc[] =
    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";

static int write_text_file(const wchar_t *path, const char *text)
{
    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD written = 0u;
    if (file == INVALID_HANDLE_VALUE) return 0;
    if (!WriteFile(file, text, (DWORD)strlen(text), &written, NULL) || written != strlen(text)) {
        CloseHandle(file); return 0;
    }
    CloseHandle(file);
    return 1;
}

static int write_repeated_file(const wchar_t *path, char byte, DWORD count)
{
    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);
    char block[1024];
    DWORD written = 0u;
    memset(block, byte, sizeof(block));
    if (file == INVALID_HANDLE_VALUE) return 0;
    while (count != 0u) {
        DWORD chunk = count > sizeof(block) ? sizeof(block) : count;
        if (!WriteFile(file, block, chunk, &written, NULL) || written != chunk) {
            CloseHandle(file);
            return 0;
        }
        count -= chunk;
    }
    CloseHandle(file);
    return 1;
}

static int selection_is_zero(const byob_profile_selection *selection)
{
    return selection->ntio.file_name[0] == L'\0' && selection->ntio.bytes == 0u &&
        selection->ntdos.file_name[0] == L'\0' && selection->ntdos.bytes == 0u &&
        selection->command.file_name[0] == L'\0' && selection->command.bytes == 0u &&
        selection->target.file_name[0] == L'\0' && selection->target.bytes == 0u &&
        selection->machine_observation_count == 0u &&
        selection->has_command_placement == 0u &&
        selection->command_placement.path[0] == L'\0' &&
        selection->command_placement.drive_index == 0u &&
        selection->has_guest_boot_files == 0u &&
        selection->config_file.path[0] == L'\0' &&
        selection->autoexec_file.path[0] == L'\0';
}

int main(void)
{
    wchar_t root[MAX_PATH], ntio[MAX_PATH], ntdos[MAX_PATH], command[MAX_PATH], himem[MAX_PATH], manifest[MAX_PATH];
    wchar_t target[MAX_PATH], quit[MAX_PATH];
    char json[4096], valid_json[4096], snapshot_json[4096], inventory_json[4096], v5_json[4096], v6_json[4096], v7_json[4096], *marker;
    byob_profile_selection selection;
    byob_image image = { NULL, 0u };
    int failed = 0;
    if (GetTempPathW(MAX_PATH, root) == 0 || GetTempFileNameW(root, L"n64", 0u, root) == 0 ||
        !DeleteFileW(root) || !CreateDirectoryW(root, NULL)) return 1;
    swprintf(ntio, MAX_PATH, L"%ls\\NTIO.SYS", root);
    swprintf(ntdos, MAX_PATH, L"%ls\\NTDOS.SYS", root);
    swprintf(command, MAX_PATH, L"%ls\\COMMAND.COM", root);
    swprintf(himem, MAX_PATH, L"%ls\\HIMEM.SYS", root);
    swprintf(target, MAX_PATH, L"%ls\\TARGET.COM", root);
    swprintf(quit, MAX_PATH, L"%ls\\QUIT.COM", root);
    swprintf(manifest, MAX_PATH, L"%ls\\profile.json", root);
    failed |= !write_text_file(ntio, "abc") || !write_text_file(ntdos, "abc") || !write_text_file(command, "abc");
    snprintf(json, sizeof(json),
        "{\"features\":[],\"owner_note\":null,\"locale\":\"en-US\",\"components\":["
        "{\"sha256\":\"%s\",\"bytes\":3,\"required\":true,\"file_name\":\"COMMAND.COM\",\"role\":\"command\",\"version\":null},"
        "{\"required\":true,\"role\":\"ntio\",\"version\":null,\"bytes\":3,\"sha256\":\"%s\",\"file_name\":\"NTIO.SYS\"},"
        "{\"file_name\":\"NTDOS.SYS\",\"version\":null,\"sha256\":\"%s\",\"role\":\"ntdos\",\"bytes\":3,\"required\":true}],"
        "\"compatibility_group\":\"owned-test-set\",\"architecture\":\"x86\","
        "\"profile\":\"nt4-en-us-command-smoke-v1\",\"schema\":\"ntdos64-byob-profile-v1\"}",
        sha256_abc, sha256_abc, sha256_abc);
    strcpy(valid_json, json);
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED;
    failed |= wcscmp(selection.ntio.file_name, L"NTIO.SYS") != 0 || selection.ntio.bytes != 3u;
    failed |= selection.machine_observation_count != 0u;
    snprintf(json, sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v2\",\"profile\":\"nt4-en-us-command-smoke-v2\","
        "\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\","
        "\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],"
        "\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},"
        "\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}}}",
        sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED ||
        selection.has_command_placement != 1u || selection.command_placement.drive_index != 2u ||
        wcscmp(selection.command_placement.path, L"\\COMMAND.COM") != 0 ||
        selection.has_guest_boot_files != 1u ||
        wcscmp(selection.config_file.path, L"\\CONFIG.SYS") != 0 ||
        wcscmp(selection.autoexec_file.path, L"\\AUTOEXEC.BAT") != 0;
    failed |= !write_text_file(target, "abc");
    snprintf(json, sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v3\",\"profile\":\"nt4-en-us-command-smoke-v3\","
        "\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\","
        "\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"target\",\"file_name\":\"TARGET.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],"
        "\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},"
        "\"guest_target_placement\":{\"path\":\"\\\\TARGET.COM\",\"drive_index\":2},"
        "\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}}}",
        sha256_abc, sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_text_file(manifest, json) ||
        byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED ||
        wcscmp(selection.target.file_name, L"TARGET.COM") != 0 ||
        selection.has_target_placement != 1u || selection.target_placement.drive_index != 2u ||
        wcscmp(selection.target_placement.path, L"\\TARGET.COM") != 0;
    snprintf(json, sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v4\",\"profile\":\"nt4-en-us-command-smoke-v4\","
        "\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\","
        "\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"target\",\"file_name\":\"TARGET.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],"
        "\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},"
        "\"guest_target_placement\":{\"path\":\"\\\\TARGET.COM\",\"drive_index\":2},"
        "\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}},"
        "\"guest_search_metadata\":{\"command\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":33},\"target\":{\"attributes\":32,\"dos_time\":2,\"dos_date\":34},\"config\":{\"attributes\":32,\"dos_time\":3,\"dos_date\":35},\"autoexec\":{\"attributes\":32,\"dos_time\":4,\"dos_date\":36}}}",
        sha256_abc, sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_text_file(manifest, json) ||
        byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED ||
        selection.has_guest_search_metadata != 1u ||
        selection.command_metadata.attributes != 32u ||
        selection.target_metadata.dos_time != 2u ||
        selection.config_metadata.dos_date != 35u ||
        selection.autoexec_metadata.dos_time != 4u;
    failed |= !write_text_file(quit, "\xc4\xc4\xfe");
    snprintf(v5_json, sizeof(v5_json),
        "{\"schema\":\"ntdos64-byob-profile-v5\",\"profile\":\"nt4-en-us-command-smoke-v5\","
        "\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\","
        "\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"target\",\"file_name\":\"TARGET.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"terminal-quit\",\"file_name\":\"QUIT.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"06a37dff559df7325de8b003f4df53c188f733e0ca312aad961c34dae48d7b83\",\"version\":null}],"
        "\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},"
        "\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}},"
        "\"guest_declared_targets\":[{\"role\":\"target\",\"placement\":{\"path\":\"\\\\TARGET.COM\",\"drive_index\":2}},{\"role\":\"terminal-quit\",\"placement\":{\"path\":\"\\\\QUIT.COM\",\"drive_index\":2}}],"
        "\"guest_search_metadata\":{\"command\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":33},\"target\":{\"attributes\":32,\"dos_time\":2,\"dos_date\":34},\"terminal-quit\":{\"attributes\":32,\"dos_time\":3,\"dos_date\":35},\"config\":{\"attributes\":32,\"dos_time\":4,\"dos_date\":36},\"autoexec\":{\"attributes\":32,\"dos_time\":5,\"dos_date\":37}}}",
        sha256_abc, sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_text_file(manifest, v5_json) ||
        byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED;
    failed |=
        selection.declared_target_count != 2u || selection.declared_targets[0].terminal != 0u ||
        selection.declared_targets[1].terminal != 1u ||
        !selection.has_target_placement ||
        wcscmp(selection.target_placement.path, L"\\TARGET.COM") != 0 ||
        !selection.has_command_placement ||
        wcscmp(selection.command_placement.path, L"\\COMMAND.COM") != 0 ||
        !selection.has_guest_boot_files ||
        wcscmp(selection.config_file.path, L"\\CONFIG.SYS") != 0 ||
        wcscmp(selection.autoexec_file.path, L"\\AUTOEXEC.BAT") != 0 ||
        wcscmp(selection.declared_targets[0].placement.path, L"\\TARGET.COM") != 0 ||
        wcscmp(selection.declared_targets[1].placement.path, L"\\QUIT.COM") != 0 ||
        wcscmp(selection.terminal_quit.file_name, L"QUIT.COM") != 0;
    marker = strstr(v5_json, "terminal-quit");
    if (marker == NULL) failed = 1; else marker[0] = 'x';
    failed |= !write_text_file(manifest, v5_json) ||
        byob_profile_validate_file(manifest, root) != BYOB_PROFILE_ROLE_MISSING_OR_DUPLICATE;
    snprintf(v6_json, sizeof(v6_json),
        "{\"schema\":\"ntdos64-byob-profile-v6\",\"profile\":\"nt4-en-us-command-normal-return-v6\","
        "\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\","
        "\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"target\",\"file_name\":\"TARGET.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],"
        "\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},"
        "\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}},"
        "\"guest_declared_targets\":[{\"role\":\"target\",\"placement\":{\"path\":\"\\\\TARGET.COM\",\"drive_index\":2}}],"
        "\"guest_search_metadata\":{\"command\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":33},\"target\":{\"attributes\":32,\"dos_time\":2,\"dos_date\":34},\"config\":{\"attributes\":32,\"dos_time\":3,\"dos_date\":35},\"autoexec\":{\"attributes\":32,\"dos_time\":4,\"dos_date\":36}}}",
        sha256_abc, sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_text_file(manifest, v6_json) ||
        byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED ||
        selection.declared_target_count != 1u || selection.declared_targets[0].terminal != 0u ||
        wcscmp(selection.declared_targets[0].placement.path, L"\\TARGET.COM") != 0 ||
        selection.terminal_quit.file_name[0] != L'\0' ||
        selection.guest_display_state != BYOB_GUEST_DISPLAY_STATE_NONE;
    snprintf(v7_json, sizeof(v7_json),
        "{\"schema\":\"ntdos64-byob-profile-v7\",\"profile\":\"nt4-en-us-cli-stream-v7\","
        "\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\","
        "\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"target\",\"file_name\":\"TARGET.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],"
        "\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},"
        "\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}},"
        "\"guest_declared_targets\":[{\"role\":\"target\",\"placement\":{\"path\":\"\\\\TARGET.COM\",\"drive_index\":2}}],"
        "\"guest_search_metadata\":{\"command\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":33},\"target\":{\"attributes\":32,\"dos_time\":2,\"dos_date\":34},\"config\":{\"attributes\":32,\"dos_time\":3,\"dos_date\":35},\"autoexec\":{\"attributes\":32,\"dos_time\":4,\"dos_date\":36}},\"guest_display_state\":\"stream-io-v1\"}",
        sha256_abc, sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_text_file(manifest, v7_json) ||
        byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED ||
        selection.guest_display_state != BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1;
    marker = strstr(v7_json, "stream-io-v1");
    if (marker == NULL) failed = 1; else marker[0] = 'x';
    failed |= !write_text_file(manifest, v7_json) ||
        byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    marker = strstr(json, "\"guest_search_metadata\"");
    if (marker == NULL) failed = 1; else marker[0] = 'x';
    failed |= !write_text_file(manifest, json) ||
        byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    marker = strstr(json, "\\\\TARGET.COM");
    if (marker == NULL) failed = 1; else marker[2] = 'X';
    failed |= !write_text_file(manifest, json) ||
        byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    marker = strstr(json, "\\\\COMMAND.COM");
    if (marker == NULL) failed = 1; else marker[2] = 'X';
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_FORMAT_INVALID ||
        !selection_is_zero(&selection);
    failed |= !write_text_file(manifest,
        "{\"schema\":\"ntdos64-byob-profile-v2\",\"profile\":\"nt4-en-us-command-smoke-v2\","
        "\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\",\"components\":[],\"features\":[],\"owner_note\":null}");
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    snprintf(inventory_json, sizeof(inventory_json),
        "%.*s,\"host_drive_inventory\":{\"types\":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}}",
        (int)(strlen(valid_json) - 1u), valid_json);
    failed |= !write_text_file(manifest, inventory_json);
    failed |= byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED ||
        selection.has_host_drive_inventory != 1u || selection.host_drive_inventory.types[0] != 0u ||
        selection.host_drive_inventory.types[25] != 0u;
    marker = strstr(inventory_json, "0,0]}}");
    if (marker == NULL) failed = 1; else marker[2] = '7';
    failed |= !write_text_file(manifest, inventory_json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    failed |= byob_image_load_exact(root, &selection.ntio, &image) != BYOB_IMAGE_OK ||
        image.byte_count != 3u || memcmp(image.bytes, "abc", 3u) != 0;
    byob_image_release(&image);
    snprintf(json, sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v1\",\"profile\":\"nt4-en-us-command-smoke-v1\","
        "\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\","
        "\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],"
        "\"features\":[],\"owner_note\":null,\"machine_observations\":[{\"id\":7,\"physical_address\":1808,\"byte_count\":4}]}",
        sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED;
    failed |= selection.machine_observation_count != 1u ||
        selection.machine_observations[0].id != 7u ||
        selection.machine_observations[0].physical_address != 1808u ||
        selection.machine_observations[0].byte_count != 4u;
    strcpy(valid_json, json);
    marker = strstr(json, "}]}" );
    if (marker == NULL) failed = 1; else {
        strcpy(marker, "}],\"machine_observation_trigger\":{\"observation_id\":7,\"exception_vector\":6,\"instruction_bytes\":\"0f0b\",\"resume_bytes\":2}}");
    }
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED;
    failed |= selection.has_machine_observation_trigger != 1u ||
        selection.machine_observation_trigger.observation_id != 7u ||
        selection.machine_observation_trigger.exception_vector != 6u ||
        selection.machine_observation_trigger.instruction_bytes[0] != 0x0fu ||
        selection.machine_observation_trigger.instruction_bytes[1] != 0x0bu ||
        selection.machine_observation_trigger.resume_bytes != 2u;
    snprintf(snapshot_json, sizeof(snapshot_json),
        "{\"schema\":\"ntdos64-byob-profile-v1\",\"profile\":\"nt4-en-us-command-smoke-v1\","
        "\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\","
        "\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],"
        "\"features\":[],\"owner_note\":null,\"machine_observations\":[{\"id\":11,\"physical_address\":64,\"byte_count\":48},{\"id\":12,\"physical_address\":1174,\"byte_count\":1},{\"id\":13,\"physical_address\":1812,\"byte_count\":4}],"
        "\"machine_startup_snapshot_trigger\":{\"observation_ids\":[11,12,13],\"exception_vector\":6,\"instruction_bytes\":\"0f0b\",\"resume_bytes\":2},\"machine_startup_snapshot_evidence_file\":\"snapshot-evidence.json\",\"machine_startup_plan\":false}",
        sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_text_file(manifest, snapshot_json);
    failed |= byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED;
    failed |= selection.has_machine_observation_trigger != 0u ||
        selection.has_machine_startup_snapshot_trigger != 1u ||
        selection.machine_startup_snapshot_trigger.observation_count != 3u ||
        selection.machine_startup_snapshot_trigger.observation_ids[0] != 11u ||
        selection.machine_startup_snapshot_trigger.observation_ids[1] != 12u ||
        selection.machine_startup_snapshot_trigger.observation_ids[2] != 13u ||
        selection.machine_startup_snapshot_trigger.exception_vector != 6u ||
        selection.machine_startup_snapshot_trigger.instruction_bytes[0] != 0x0fu ||
        selection.machine_startup_snapshot_trigger.instruction_bytes[1] != 0x0bu ||
        selection.machine_startup_snapshot_trigger.resume_bytes != 2u ||
        selection.machine_startup_plan_enabled != 0u ||
        wcscmp(selection.machine_startup_snapshot_evidence_file_name,
            L"snapshot-evidence.json") != 0;
    strcpy(json, snapshot_json);
    marker = strstr(json, "\"machine_startup_plan\":false");
    if (marker == NULL) failed = 1; else {
        marker[0] = '\0';
        strcat(json, "\"machine_startup_plan\":true,\"machine_startup_entry\":\"ntio-v0\"}");
    }
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_ACCEPTED ||
        selection.machine_startup_plan_enabled != 1u ||
        selection.machine_startup_entry_ntio_v0 != 1u;
    strcpy(json, snapshot_json);
    marker = strstr(json, "snapshot-evidence.json");
    if (marker == NULL) failed = 1; else marker[0] = '/';
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    strcpy(json, snapshot_json);
    marker = strstr(json, "snapshot-evidence.json");
    if (marker == NULL) failed = 1; else {
        marker[0] = '\0';
        memmove(marker + 1u, marker + strlen("snapshot-evidence.json"),
            strlen(marker + strlen("snapshot-evidence.json")) + 1u);
    }
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    strcpy(json, snapshot_json);
    marker = strstr(json, "[11,12,13]");
    if (marker == NULL) failed = 1; else marker[4] = '9';
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    marker = strstr(json, "\"instruction_bytes\":\"0f0b\"");
    if (marker == NULL) failed = 1; else marker[21] = 'c';
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    strcpy(json, valid_json);
    marker = strstr(json, "\"byte_count\":4");
    if (marker == NULL) failed = 1; else memcpy(marker, "\"byte_count\":0", 14u);
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    strcpy(json, valid_json);
    marker = strstr(json, "}]}" );
    if (marker == NULL) failed = 1; else {
        strcpy(marker, "},{\"id\":7,\"physical_address\":2000,\"byte_count\":1}]}" );
    }
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    strcpy(valid_json,
        "{\"features\":[],\"owner_note\":null,\"locale\":\"en-US\",\"components\":["
        "{\"sha256\":\"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad\",\"bytes\":3,\"required\":true,\"file_name\":\"COMMAND.COM\",\"role\":\"command\",\"version\":null},"
        "{\"required\":true,\"role\":\"ntio\",\"version\":null,\"bytes\":3,\"sha256\":\"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad\",\"file_name\":\"NTIO.SYS\"},"
        "{\"file_name\":\"NTDOS.SYS\",\"version\":null,\"sha256\":\"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad\",\"role\":\"ntdos\",\"bytes\":3,\"required\":true}],"
        "\"compatibility_group\":\"owned-test-set\",\"architecture\":\"x86\","
        "\"profile\":\"nt4-en-us-command-smoke-v1\",\"schema\":\"ntdos64-byob-profile-v1\"}");
    failed |= !write_text_file(manifest, valid_json);
    failed |= !write_text_file(ntio, "def");
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_COMPONENT_IDENTITY_MISMATCH;
    failed |= !write_text_file(ntio, "abc");
    strcpy(json, valid_json);
    marker = strstr(json, "nt4-en-us-command-smoke-v1");
    if (marker == NULL) failed = 1; else marker[0] = 'x';
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_TARGET_MISMATCH;
    strcpy(json, valid_json);
    marker = strstr(json, "NTIO.SYS");
    if (marker == NULL) failed = 1; else marker[0] = '.';
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_COMPONENT_NAME_UNSAFE;
    failed |= !write_text_file(manifest, "{\"schema\":\"ntdos64-byob-profile-v1\",\"schema\":\"ntdos64-byob-profile-v1\"}");
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    snprintf(json, sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v1\",\"profile\":\"nt4-en-us-command-smoke-v1\","
        "\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"\","
        "\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],\"features\":[],\"owner_note\":null}",
        sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_text_file(manifest, json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_COMPATIBILITY_GROUP_MISMATCH;
    snprintf(json, sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v1\",\"profile\":\"nt4-en-us-command-smoke-v1\","
        "\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\","
        "\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},"
        "{\"role\":\"himem\",\"file_name\":\"HIMEM.SYS\",\"required\":false,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],\"features\":[],\"owner_note\":null}",
        sha256_abc, sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_text_file(himem, "abc") || !write_text_file(manifest, json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FEATURE_DECLARATION_INVALID;
    failed |= !write_text_file(manifest,
        "{\"schema\":\"ntdos64-byob-profile-v1\",\"profile\":\"nt4-en-us-command-smoke-v1\",\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\",\"components\":[],\"features\":[],\"owner_note\":null}");
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_ROLE_MISSING_OR_DUPLICATE;
    failed |= !write_repeated_file(manifest, ' ', 65537u);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    failed |= !DeleteFileW(ntio) || !write_text_file(manifest, valid_json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_COMPONENT_OPEN_REJECTED;
    failed |= !write_text_file(ntio, "") || !write_text_file(manifest, valid_json);
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_COMPONENT_OPEN_REJECTED;
    failed |= !write_text_file(ntio, "abc");
    failed |= !DeleteFileW(ntio) || !CreateDirectoryW(ntio, NULL) || !write_text_file(manifest, valid_json);
    memset(&selection, 0xa5, sizeof(selection));
    failed |= byob_profile_validate_file_select(manifest, root, &selection) != BYOB_PROFILE_COMPONENT_OPEN_REJECTED;
    failed |= !selection_is_zero(&selection);
    RemoveDirectoryW(ntio);
    failed |= !write_text_file(ntio, "abc");
    failed |= !write_text_file(manifest,
        "{\"schema\":\"ntdos64-byob-profile-v1\",\"profile\":\"nt4-en-us-command-smoke-v1\",\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"owned-test-set\",\"components\":[],\"features\":[],\"owner_note\":null}\xFF");
    failed |= byob_profile_validate_file(manifest, root) != BYOB_PROFILE_FORMAT_INVALID;
    DeleteFileW(manifest); DeleteFileW(quit); DeleteFileW(target); DeleteFileW(himem); DeleteFileW(command); DeleteFileW(ntdos); DeleteFileW(ntio); RemoveDirectoryW(root);
    if (failed) return 1;
    puts("byob-profile-test: owned BYOB profile acceptance and rejection families verified");
    return 0;
}
