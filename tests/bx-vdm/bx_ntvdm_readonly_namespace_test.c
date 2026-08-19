#include "bop-v1/bx_ntvdm_readonly_namespace.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    uint8_t command[] = { 1u, 2u, 3u }, target[] = { 4u, 5u }, received[32];
    byob_image image = { command, sizeof(command) };
    byob_image target_image = { target, sizeof(target) };
    byob_profile_selection selection = { 0 };
    bx_ntvdm_readonly_namespace_v1 value;
    uint32_t token, stale, count;
    uint64_t size;
    wcscpy(selection.command_placement.path, L"\\COMMAND.COM");
    selection.command_placement.drive_index = 2u;
    selection.has_command_placement = 1u;
    wcscpy(selection.config_file.path, L"\\CONFIG.SYS");
    selection.config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    wcscpy(selection.autoexec_file.path, L"\\AUTOEXEC.BAT");
    selection.autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    selection.has_guest_boot_files = 1u;
    if (!bx_ntvdm_readonly_namespace_v1_initialize(&value, &image, &selection) ||
        bx_ntvdm_readonly_namespace_v1_open(&value, 1u, L"\\COMMAND.COM", &token, &size) ||
        bx_ntvdm_readonly_namespace_v1_open(&value, 2u, L"\\OTHER.COM", &token, &size) ||
        !bx_ntvdm_readonly_namespace_v1_open(&value, 2u, L"\\COMMAND.COM", &token, &size) ||
        size != sizeof(command) ||
        !bx_ntvdm_readonly_namespace_v1_read(&value, token, received, 2u, &count) ||
        count != 2u || memcmp(received, command, 2u) != 0 ||
        !bx_ntvdm_readonly_namespace_v1_seek(&value, token, 1u) ||
        !bx_ntvdm_readonly_namespace_v1_read(&value, token, received, sizeof(received), &count) ||
        count != 2u || memcmp(received, command + 1u, 2u) != 0 ||
        !bx_ntvdm_readonly_namespace_v1_close(&value, token) ||
        bx_ntvdm_readonly_namespace_v1_close(&value, token) ||
        !bx_ntvdm_readonly_namespace_v1_open(&value, 2u, L"\\CONFIG.SYS", &stale, &size) ||
        size != 0u || !bx_ntvdm_readonly_namespace_v1_read(&value, stale, received, sizeof(received), &count) ||
        count != 0u || !bx_ntvdm_readonly_namespace_v1_close(&value, stale) ||
        !bx_ntvdm_readonly_namespace_v1_open(&value, 2u, L"\\AUTOEXEC.BAT", &stale, &size) ||
        size != 0u || !bx_ntvdm_readonly_namespace_v1_read(&value, stale, received, sizeof(received), &count) ||
        count != 0u || !bx_ntvdm_readonly_namespace_v1_close(&value, stale) ||
        bx_ntvdm_readonly_namespace_v1_read(&value, token, received, 1u, &count)) return 1;
    wcscpy(selection.target.file_name, L"TARGET.COM");
    wcscpy(selection.target_placement.path, L"\\TARGET.COM");
    selection.target_placement.drive_index = 2u;
    selection.has_target_placement = 1u;
    selection.declared_target_count = 1u;
    wcscpy(selection.declared_targets[0].component.file_name, L"TARGET.COM");
    wcscpy(selection.declared_targets[0].placement.path, L"\\TARGET.COM");
    selection.declared_targets[0].placement.drive_index = 2u;
    if (!bx_ntvdm_readonly_namespace_v1_append_target(&value, &target_image, &selection) ||
        !bx_ntvdm_readonly_namespace_v1_open(&value, 2u, L"\\COMMAND.COM", &token, &size) ||
        !bx_ntvdm_readonly_namespace_v1_open(&value, 2u, L"\\TARGET.COM", &stale, &size) ||
        size != sizeof(target) || value.open != 2u ||
        !bx_ntvdm_readonly_namespace_v1_read(&value, token, received, 1u, &count) ||
        count != 1u || received[0] != command[0] ||
        !bx_ntvdm_readonly_namespace_v1_read(&value, stale, received, sizeof(received), &count) ||
        count != sizeof(target) || memcmp(received, target, sizeof(target)) != 0 ||
        !bx_ntvdm_readonly_namespace_v1_read(&value, token, received, sizeof(received), &count) ||
        count != sizeof(command) - 1u || memcmp(received, command + 1u, count) != 0 ||
        !bx_ntvdm_readonly_namespace_v1_close(&value, stale) ||
        bx_ntvdm_readonly_namespace_v1_read(&value, stale, received, 1u, &count) ||
        !bx_ntvdm_readonly_namespace_v1_close(&value, token) || value.open != 0u ||
        bx_ntvdm_readonly_namespace_v1_append_target(&value, &target_image, &selection)) return 1;
    puts("bx-ntvdm-readonly-namespace-test: concurrent immutable file handles verified");
    return 0;
}
