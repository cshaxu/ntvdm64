#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "bx_ntvdm_command_profile_consumer_v1.h"
#include "bx_ntvdm_startup_configuration_source_v1.h"

static int write_bytes(const wchar_t *path, const void *bytes, DWORD byte_count)
{
    HANDLE file;
    DWORD written = 0u;
    file = CreateFileW(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE) return 0;
    if (!WriteFile(file, bytes, byte_count, &written, 0) || written != byte_count) {
        CloseHandle(file);
        return 0;
    }
    CloseHandle(file);
    return 1;
}

int main(void)
{
    wchar_t root[MAX_PATH], config[MAX_PATH], autoexec[MAX_PATH];
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_startup_configuration_input_v1 input;
    char oversized[4096];
    int failed = 0;

    if (GetTempPathW(MAX_PATH, root) == 0 ||
        GetTempFileNameW(root, L"n64", 0u, root) == 0 ||
        !DeleteFileW(root) || !CreateDirectoryW(root, 0)) return 1;
    swprintf(config, MAX_PATH, L"%ls\\CONFIG.SYS", root);
    swprintf(autoexec, MAX_PATH, L"%ls\\AUTOEXEC.BAT", root);
    memset(oversized, 'x', sizeof(oversized));
    bx_ntvdm_mutation_profile_v1_initialize(&profile,
        BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    failed |= !bx_ntvdm_command_profile_consumer_v1_register_class(&profile,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu);

    failed |= !write_bytes(config, "FILES=20\r\n", 10u) ||
        !write_bytes(autoexec, "", 0u) ||
        !SetEnvironmentVariableW(L"NTDOS64_STARTUP_CONFIG_SOURCE", config) ||
        !SetEnvironmentVariableW(L"NTDOS64_STARTUP_AUTOEXEC_SOURCE", autoexec) ||
        !bx_ntvdm_startup_configuration_source_v1_from_environment(&input, &profile) ||
        input.config_bytes != 10u || memcmp(input.config, "FILES=20\r\n", 10u) != 0 ||
        input.autoexec_bytes != 0u || !bx_ntvdm_startup_configuration_input_v1_valid(&input);

    failed |= !SetEnvironmentVariableW(L"NTDOS64_STARTUP_AUTOEXEC_SOURCE", 0) ||
        bx_ntvdm_startup_configuration_source_v1_from_environment(&input, &profile) != 0;
    failed |= !SetEnvironmentVariableW(L"NTDOS64_STARTUP_AUTOEXEC_SOURCE", autoexec) ||
        !write_bytes(config, "", 0u) ||
        bx_ntvdm_startup_configuration_source_v1_from_environment(&input, &profile) != 0;
    failed |= !write_bytes(config, oversized, sizeof(oversized)) ||
        bx_ntvdm_startup_configuration_source_v1_from_environment(&input, &profile) != 0;

    SetEnvironmentVariableW(L"NTDOS64_STARTUP_CONFIG_SOURCE", 0);
    SetEnvironmentVariableW(L"NTDOS64_STARTUP_AUTOEXEC_SOURCE", 0);
    DeleteFileW(autoexec);
    DeleteFileW(config);
    RemoveDirectoryW(root);
    return failed ? 1 : 0;
}
