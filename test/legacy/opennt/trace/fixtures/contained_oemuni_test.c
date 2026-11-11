#include <windows.h>
#include <string.h>
#include "oemuni.h"

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

BOOL runner_oemuni_configure_root(LPCSTR);
VOID runner_oemuni_reset(VOID);

int main(void) {
    char temp[MAX_PATH], root[MAX_PATH], value[32];
    HANDLE file, find;
    DWORD written;
    WIN32_FIND_DATAA data;

    if (!GetTempPathA(MAX_PATH, temp) || !GetTempFileNameA(temp, "noe", 0, root)) return 1;
    if (!DeleteFileA(root) || !CreateDirectoryA(root, NULL)) return 2;
    if (!runner_oemuni_configure_root(root)) { RemoveDirectoryA(root); return 3; }
    file = CreateFileOem("C:\\note.txt", GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !WriteFile(file, "ok", 2, &written, NULL) || written != 2) { if (file != INVALID_HANDLE_VALUE) CloseHandle(file); runner_oemuni_reset(); RemoveDirectoryA(root); return 4; }
    CloseHandle(file);
    if (GetFileAttributesOem("C:\\note.txt") == INVALID_FILE_ATTRIBUTES || CreateFileOem("C:\\..\\escape.txt", GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL) != INVALID_HANDLE_VALUE) { runner_oemuni_reset(); DeleteFileA(root); RemoveDirectoryA(root); return 5; }
    find = FindFirstFileOem("C:\\*.txt", &data);
    if (find == INVALID_HANDLE_VALUE || strcmp(data.cFileName, "note.txt") != 0) { if (find != INVALID_HANDLE_VALUE) FindClose(find); runner_oemuni_reset(); DeleteFileA(root); RemoveDirectoryA(root); return 6; }
    FindClose(find);
    if (!MoveFileOem("C:\\note.txt", "C:\\moved.txt") || !SetEnvironmentVariableOem("=C:", "C:\\") || GetEnvironmentVariableOem("=C:", value, sizeof(value)) != 3 || strcmp(value, "C:\\") != 0 || !DeleteFileOem("C:\\moved.txt")) { runner_oemuni_reset(); DeleteFileA(root); RemoveDirectoryA(root); return 7; }
    runner_oemuni_reset();
    if (CreateFileOem("C:\\after-reset.txt", GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL) != INVALID_HANDLE_VALUE || !RemoveDirectoryA(root)) return 8;
    return 0;
}
