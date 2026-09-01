#include "mvdm_softpc_termination.h"

#include <stdio.h>
#include <string.h>

#include <windows.h>

int main(void)
{
    char temp_path[MAX_PATH];
    char report_path[MAX_PATH];
    char report[160];
    FILE *file;
    size_t bytes_read;

    if (GetTempPathA((DWORD)sizeof(temp_path), temp_path) == 0u ||
        GetTempFileNameA(temp_path, "t35", 0u, report_path) == 0u ||
        !DeleteFileA(report_path)) return 10;

    mvdm_softpc_record_config_command_store(0x914e6u, (uint8_t)'S');
    if (GetFileAttributesA(report_path) != INVALID_FILE_ATTRIBUTES) return 11;

    if (!SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_STORE_REPORT_PATH",
            report_path) || !SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_LINEAR",
            "0x914e6")) return 12;
    mvdm_softpc_record_config_command_store(0x914e5u, (uint8_t)'S');
    mvdm_softpc_record_config_command_store(0x914e6u, (uint8_t)'S');
    mvdm_softpc_record_config_command_store(0x914e6u, (uint8_t)'X');
    SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_STORE_REPORT_PATH", NULL);
    SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_LINEAR", NULL);

    if (fopen_s(&file, report_path, "rb") != 0 || file == NULL) return 13;
    bytes_read = fread(report, 1u, sizeof(report) - 1u, file);
    fclose(file);
    report[bytes_read] = '\0';
    DeleteFileA(report_path);
    return strstr(report,
        "ordinal=1 linear=914E6 value=53 state=copied") != NULL &&
        strstr(report,
            "ordinal=2 linear=914E6 value=58 state=copied") != NULL ? 0 : 14;
}
