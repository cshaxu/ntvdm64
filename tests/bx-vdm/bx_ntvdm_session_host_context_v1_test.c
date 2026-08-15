#include "bx_ntvdm_session_host_context_v1.h"

#include <string.h>
#include <stdio.h>
#include <windows.h>

static int profile(uint32_t mode, bx_ntvdm_mutation_profile_v1 *value)
{
    bx_ntvdm_mutation_profile_v1_initialize(value, mode);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(value,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu);
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 direct_profile, readonly_profile, overlay_profile, virtual_profile;
    bx_ntvdm_session_host_context_v1 direct, readonly, overlay, virtual_context;
    bx_ntvdm_host_drive_snapshot_v1 snapshot;
    bx_ntvdm_host_namespace_v1 space;
    bx_ntvdm_command_host_context_v1 command;
    wchar_t original[MAX_PATH], relative[BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE];
    uint8_t environment[] = { 'P','A','T','H','=', 0u,':','\\', 0u, 0u };
    uint8_t directory[] = { 0u, ':', '\\', 0u };
    uint8_t drive;
    uint32_t error = ERROR_INVALID_PARAMETER;
    int failed = 0;

    if (GetCurrentDirectoryW(MAX_PATH, original) == 0u || original[0] < L'A' ||
        original[0] > L'Z' || original[1] != L':') return 2;
    drive = (uint8_t)(original[0] - L'A');
    environment[5] = (uint8_t)original[0];
    directory[0] = (uint8_t)original[0];
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = snapshot.admitted_mask = UINT32_C(1) << drive;
    snapshot.types[drive] = DRIVE_FIXED;
    if (!bx_ntvdm_host_namespace_v1_initialize(&space, &snapshot) ||
        !profile(BX_NTVDM_MUTATION_MODE_V1_DIRECT, &direct_profile) ||
        !bx_ntvdm_session_host_context_v1_initialize(&direct, &direct_profile, &space, drive)) return 3;
    if (bx_ntvdm_session_host_context_v1_set(&direct, drive, L"", &error) !=
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK) { printf("direct set %lu\n", (unsigned long)error); failed = 1; }
    if (error != ERROR_SUCCESS) { printf("direct error %lu\n", (unsigned long)error); failed = 1; }
    if (!bx_ntvdm_session_host_context_v1_set_environment(&direct, environment,
            (uint32_t)sizeof(environment))) { puts("direct environment"); failed = 1; }
    if (!bx_ntvdm_command_host_context_v1_initialize(&command, drive, directory, 3u) ||
        !bx_ntvdm_command_host_context_v1_set_environment(&command, environment,
            (uint32_t)sizeof(environment)) ||
        !bx_ntvdm_session_host_context_v1_seed_command(&direct, &command) ||
        bx_ntvdm_session_host_context_v1_query(&direct, drive, relative,
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE) !=
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK || relative[0] != L'\0') {
        puts("direct root seed"); failed = 1;
    }
    if (!bx_ntvdm_session_host_context_v1_project_command(&direct, &command)) { puts("direct projection"); failed = 1; }
    if (command.selected_drive != drive || memcmp(command.selected_directory, directory, 4u) != 0 ||
        command.environment_bytes != sizeof(environment) ||
        memcmp(command.environment, environment, sizeof(environment)) != 0) { puts("direct copy"); failed = 1; }
    bx_ntvdm_session_host_context_v1_teardown(&direct);
    bx_ntvdm_host_namespace_v1_release(&space);
    if (!SetCurrentDirectoryW(original)) return 4;

    if (!profile(BX_NTVDM_MUTATION_MODE_V1_READONLY, &readonly_profile) ||
        !bx_ntvdm_session_host_context_v1_initialize(&readonly, &readonly_profile, 0, drive) ||
        bx_ntvdm_session_host_context_v1_set(&readonly, drive, L"", &error) !=
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_READONLY) { puts("readonly"); failed = 1; }

    if (!profile(BX_NTVDM_MUTATION_MODE_V1_OVERLAY, &overlay_profile) ||
        !bx_ntvdm_session_host_context_v1_initialize(&overlay, &overlay_profile, 0, drive) ||
        bx_ntvdm_session_host_context_v1_set(&overlay, drive, L"WORK", &error) !=
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK ||
        bx_ntvdm_session_host_context_v1_query(&overlay, drive, relative,
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE) != BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK ||
        wcscmp(relative, L"WORK") != 0 ||
        bx_ntvdm_session_host_context_v1_set_default_drive(&overlay, drive) !=
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK) { puts("overlay"); failed = 1; }
    bx_ntvdm_session_host_context_v1_teardown(&overlay);

    if (!profile(BX_NTVDM_MUTATION_MODE_V1_VIRTUAL, &virtual_profile) ||
        !bx_ntvdm_session_host_context_v1_initialize(&virtual_context, &virtual_profile, 0, drive) ||
        bx_ntvdm_session_host_context_v1_set(&virtual_context, drive, L"VIRT", &error) !=
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK ||
        bx_ntvdm_session_host_context_v1_query(&virtual_context, drive, relative,
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE) != BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK ||
        wcscmp(relative, L"VIRT") != 0) { puts("virtual"); failed = 1; }
    bx_ntvdm_session_host_context_v1_teardown(&virtual_context);
    return failed ? 1 : 0;
}
