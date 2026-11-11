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
    length = ExpandEnvironmentStringsOem("%S37_OEM_FIXTURE%", path, sizeof(path));
    if (length != strlen(expected)+1 || strcmp(path, expected)) {
        printf("S37_OEM_EXPAND_MISMATCH actual=%02x expected=%02x length=%lu\n",
            (unsigned char)path[0], (unsigned char)expected[0], length);
        return 13;
    }
    puts("S37_OEM_EXPAND_NONASCII_OK");
    memset(path, 0x5a, sizeof(path));
    length = ExpandEnvironmentStringsOem("%S37_OEM_FIXTURE%", path, 1);
    /* Preserve the reached ANSI API's insufficient-buffer sizing contract;
     * its required count need not equal the successful OEM byte count. */
    if (length != ExpandEnvironmentStringsA("%S37_OEM_FIXTURE%", expected, 1) ||
        length <= 1 || (unsigned char)path[1] != 0x5a) return 14;
    SetLastError(0);
    if (ExpandEnvironmentStringsOem(NULL, path, sizeof(path)) ||
        GetLastError() != ERROR_INVALID_PARAMETER) return 15;
    if (!SetEnvironmentVariableOem("S37_OEM_FIXTURE", NULL)) return 11;
    SetLastError(0);
    if (GetEnvironmentVariableOem("S37_OEM_FIXTURE", path, sizeof(path)) ||
        GetLastError() != ERROR_ENVVAR_NOT_FOUND) return 12;
    puts("S37_OEM_ENV_NONASCII_DELETE_MISSING_OK");
    {
        static CHAR large[65536];
        DWORD native_length, oem_length;
        if (!SetEnvironmentVariableW(L"S37_OEM_MATRIX", L"abc")) return 16;
        memset(large, 0x5a, sizeof(large));
        SetLastError(0);
        oem_length = GetEnvironmentVariableOem("S37_OEM_MATRIX", large, sizeof(large));
        printf("S37_ENV_LARGE result=%lu expected=3 error=%lu copied=%d\n",
            oem_length, GetLastError(), !memcmp(large,"abc",4));
        if (oem_length != 3 || memcmp(large,"abc",4)) return 17;
        memset(path,0x5a,sizeof(path));
        native_length=GetEnvironmentVariableA("S37_OEM_MATRIX",expected,1);
        oem_length=GetEnvironmentVariableOem("S37_OEM_MATRIX",path,1);
        if (oem_length!=native_length || path[1]!=0x5a) return 18;
        if (!SetEnvironmentVariableW(L"S37_OEM_MATRIX", L"")) return 19;
        SetLastError(0);
        native_length=GetEnvironmentVariableA("S37_OEM_MATRIX",expected,sizeof(expected));
        memset(path,0x5a,sizeof(path));
        SetLastError(0);
        oem_length=GetEnvironmentVariableOem("S37_OEM_MATRIX",path,sizeof(path));
        if (native_length || oem_length || expected[0] || path[0]) return 20;
        if (!SetEnvironmentVariableOem("S37_OEM_MATRIX",NULL)) return 21;
        puts("S37_ENV_LARGE_SHORT_EMPTY_DELETE_OK");
    }

    return 0;
}
