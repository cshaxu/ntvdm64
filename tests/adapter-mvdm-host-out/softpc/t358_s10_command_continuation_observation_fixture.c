#include "mvdm_softpc_termination.h"

#include <stdio.h>
#include <string.h>

#include <windows.h>

int main(void)
{
    char temp_path[MAX_PATH];
    char report_path[MAX_PATH];
    char report[256];
    FILE *file;
    size_t bytes_read;

    if (GetTempPathA((DWORD)sizeof(temp_path), temp_path) == 0u ||
        GetTempFileNameA(temp_path, "t35", 0u, report_path) == 0u ||
        !DeleteFileA(report_path)) return 10;
    mvdm_softpc_record_command_continuation(0u, 0x0070u, 0x0479u,
        0x0000u, 0x0012u, 0u, 1u, 0u, 1u);
    if (GetFileAttributesA(report_path) != INVALID_FILE_ATTRIBUTES) return 11;
    if (!SetEnvironmentVariableA("MVDM_COMMAND_CONTINUATION_REPORT_PATH",
            report_path)) return 12;
    mvdm_softpc_record_command_continuation(1u, 0x0070u, 0x047Cu,
        0x0037u, 0x0020u, 1u, 0u, 1u, 2u);
    SetEnvironmentVariableA("MVDM_COMMAND_CONTINUATION_REPORT_PATH", NULL);
    if (fopen_s(&file, report_path, "rb") != 0 || file == NULL) return 13;
    bytes_read = fread(report, 1u, sizeof(report) - 1u, file);
    fclose(file);
    report[bytes_read] = '\0';
    DeleteFileA(report_path);
    return strstr(report,
        "svc=01 stage=1 cs=0070 ip=047C ax=0037 bx=0020 cf=1 first=0 repeat=1 dos-state=00000002")
        != NULL ? 0 : 14;
}
