#include "mvdm_softpc_termination.h"

#include <stdio.h>
#include <string.h>

#include <windows.h>

int main(void)
{
    char temp_path[MAX_PATH];
    char report_path[MAX_PATH];
    char report[320];
    FILE *file;
    size_t bytes_read;

    if (GetTempPathA((DWORD)sizeof(temp_path), temp_path) == 0u ||
        GetTempFileNameA(temp_path, "t35", 0u, report_path) == 0u ||
        !DeleteFileA(report_path)) return 10;
    mvdm_softpc_record_dem_read(0x00a7u, 0x03d2u, 0x0040u, 0u, 0u,
        0u, 0u, 0u, 0u);
    if (GetFileAttributesA(report_path) != INVALID_FILE_ATTRIBUTES) return 11;
    if (!SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH", report_path))
        return 12;
    mvdm_softpc_record_dem_read(0x00a7u, 0x03d2u, 0x0040u, 0x0001u,
        0x0020u, 0x0040u, 1u, 0x0040u, 0u);
    SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH", NULL);
    if (fopen_s(&file, report_path, "rb") != 0 || file == NULL) return 13;
    bytes_read = fread(report, 1u, sizeof(report) - 1u, file);
    fclose(file);
    report[bytes_read] = '\0';
    DeleteFileA(report_path);
    return strstr(report,
        "phase=1 ds=00A7 dx=03D2 requested=0040 offset=0001:0020 completed=0040 ax=0040 cf=0 state=copied")
        != NULL ? 0 : 14;
}
