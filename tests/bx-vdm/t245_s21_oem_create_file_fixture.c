#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "bop/shim/demfile_shim.h"

int main(void)
{
    char temporary[MAX_PATH];
    char file[MAX_PATH];
    HANDLE handle;
    DWORD bytes;

    if (!GetTempPathA((DWORD)sizeof(temporary), temporary) ||
        GetTempFileNameA(temporary, "oem", 0u, file) == 0u) return 1;
    handle = CreateFileOem(file, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0u, NULL);
    if (handle == INVALID_HANDLE_VALUE) { DeleteFileA(file); return 2; }
    if (!WriteFile(handle, "x", 1u, &bytes, NULL) || bytes != 1u ||
        !CloseHandle(handle)) { DeleteFileA(file); return 3; }
    handle = CreateFileOem(file, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0u, NULL);
    if (handle == INVALID_HANDLE_VALUE || !CloseHandle(handle)) {
        DeleteFileA(file);
        return 4;
    }
    if (!DeleteFileA(file)) return 5;
    puts("T245 S21 OpenNT-shaped OEM CreateFileW conversion/retry seam verified");
    return 0;
}
