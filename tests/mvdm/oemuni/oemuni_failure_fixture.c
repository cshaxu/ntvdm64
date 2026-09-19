#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "oemuni.h"

int
main(void)
{
    CHAR path[MAX_PATH];
    LPSTR file_part;
    WCHAR wide_name[32] = L"oem_\x00e9.txt";
    WCHAR wide_full[MAX_PATH];
    WCHAR *wide_part;
    CHAR oem_name[64], expected[MAX_PATH];
    BOOL substituted = FALSE;
    DWORD length;

    SetLastError(0);
    if (GetShortPathNameOem(NULL, path, sizeof(path)) != 0u ||
        GetLastError() != ERROR_INVALID_PARAMETER) return 1;

    file_part = NULL;
    if (GetFullPathNameOem("oemuni_probe.txt", sizeof(path), path, &file_part) == 0u ||
        file_part == NULL || file_part < path ||
        file_part >= path + sizeof(path)) return 2;

    printf("OEMUNI_HOST_CODEPAGES OEM=%u ANSI=%u\n", GetOEMCP(), GetACP());
    if (!WideCharToMultiByte(CP_OEMCP, 0, wide_name, -1, oem_name,
        sizeof(oem_name), NULL, &substituted) || substituted) return 3;
    if (!GetFullPathNameW(wide_name, MAX_PATH, wide_full, &wide_part)) return 4;
    if (!WideCharToMultiByte(CP_OEMCP, 0, wide_full, -1, expected,
        sizeof(expected), NULL, NULL)) return 5;
    length = GetFullPathNameOem(oem_name, sizeof(path), path, &file_part);
    if (length != strlen(expected) || strcmp(path, expected) ||
        !file_part || strcmp(file_part, oem_name)) return 6;
    memset(path, 0x5a, sizeof(path));
    if (GetFullPathNameOem(oem_name, 1, path, NULL) != strlen(expected)+1 ||
        (unsigned char)path[0] != 0x5a) return 7;
    puts("S37_OEM_FULLPATH_NONASCII_PART_SHORT_BUFFER_OK");

    if (!SetEnvironmentVariableW(L"S37_OEM_FIXTURE", L"\x00e9")) return 8;
    substituted = FALSE;
    if (!WideCharToMultiByte(CP_OEMCP, 0, L"\x00e9", -1, expected,
        sizeof(expected), NULL, &substituted) || substituted) return 9;
    length = GetEnvironmentVariableOem("S37_OEM_FIXTURE", path, sizeof(path));
    if (!length || strcmp(path, expected)) return 10;
    if (!SetEnvironmentVariableOem("S37_OEM_FIXTURE", NULL)) return 11;
    SetLastError(0);
    if (GetEnvironmentVariableOem("S37_OEM_FIXTURE", path, sizeof(path)) ||
        GetLastError() != ERROR_ENVVAR_NOT_FOUND) return 12;
    puts("S37_OEM_ENV_NONASCII_DELETE_MISSING_OK");

    return 0;
}
