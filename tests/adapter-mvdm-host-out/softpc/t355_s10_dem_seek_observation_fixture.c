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
    if (snprintf(report, sizeof(report), "%st355-s10-dem-seek.txt", directory) <= 0)
        return 2;
    DeleteFileA(report);
    if (!SetEnvironmentVariableA("MVDM_DEM_SEEK_REPORT_PATH", report)) return 3;
    mvdm_softpc_record_dem_seek(0, 0, 2u, 0u, 0u, 0u, 0, 0);
    mvdm_softpc_record_dem_seek(0, 0, 2u, 1u, 0, 0x0040u, 0x0040u, 0);
    file = fopen(report, "rb");
    if (file == NULL) return 4;
    length = (DWORD)fread(content, 1, sizeof(content) - 1u, file);
    fclose(file);
    content[length] = '\0';
    DeleteFileA(report);
    return strstr(content, "phase=0 requested=0000:0000 origin=02") != NULL &&
        strstr(content, "phase=1 requested=0000:0000 origin=02 result=0000:0040") != NULL
        ? 0 : 5;
}
