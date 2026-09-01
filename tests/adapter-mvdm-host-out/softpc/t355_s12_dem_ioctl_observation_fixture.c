#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "mvdm_softpc_termination.h"

int main(void)
{
    char directory[MAX_PATH];
    char report[MAX_PATH];
    char content[512];
    DWORD length;
    FILE *file;

    if (GetTempPathA((DWORD)sizeof(directory), directory) == 0) return 1;
    if (snprintf(report, sizeof(report), "%st355-s12-dem-ioctl.txt", directory) <= 0)
        return 2;
    DeleteFileA(report);
    if (!SetEnvironmentVariableA("MVDM_DEM_IOCTL_REPORT_PATH", report)) return 3;
    mvdm_softpc_record_dem_ioctl(8u, 2u, 0u, 0x4408u, 0u, 0u);
    mvdm_softpc_record_dem_ioctl(8u, 2u, 1u, 1u, 0u, 0u);
    file = fopen(report, "rb");
    if (file == NULL) return 4;
    length = (DWORD)fread(content, 1, sizeof(content) - 1u, file);
    fclose(file);
    content[length] = '\0';
    DeleteFileA(report);
    return strstr(content, "phase=0 subfunction=08 drive=02 ax=4408") != NULL &&
        strstr(content, "phase=1 subfunction=08 drive=02 ax=0001") != NULL
        ? 0 : 5;
}
