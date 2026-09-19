#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "oemuni.h"

#define CHECK(x) do { if (!(x)) { printf("S37_FAMILY_FAIL line=%d error=%lu\n", __LINE__, GetLastError()); return 1; } } while (0)
int main(void)
{
    CHAR cwd[MAX_PATH], path[MAX_PATH], temp[MAX_PATH], short_path[MAX_PATH];
    CHAR root[4], volume[MAX_PATH], fs[MAX_PATH], computer[MAX_COMPUTERNAME_LENGTH+1];
    CHAR first[] = "a\x82.dat", second[] = "b\x82.dat", third[] = "c\x82.dat";
    CHAR *part;
    WCHAR original[MAX_PATH];
    HANDLE file, find;
    WIN32_FIND_DATAA data;
    DWORD n, count, serial, max_component, flags, sectors, bytes, free_clusters, total;
    CHECK(GetOEMCP() == 437);
    CHECK(GetCurrentDirectoryW(MAX_PATH, original));
    CHECK(CreateDirectoryOem("O37CASE", NULL));
    CHECK(SetCurrentDirectoryOem("O37CASE"));
    CHECK(GetCurrentDirectoryOem(sizeof(cwd), cwd) == strlen(cwd));
    memset(path, 0x5a, sizeof(path));
    CHECK(GetCurrentDirectoryOem(1, path) == strlen(cwd)+1 && path[1] == 0x5a);
    file = CreateFileOem(first, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
    CHECK(file != INVALID_HANDLE_VALUE);
    CHECK(WriteFile(file, "OEM", 3, &n, NULL) && n == 3 && CloseHandle(file));
    CHECK(MoveFileOem(first, second));
    CHECK(MoveFileExOem(second, third, 0));
    CHECK(SetFileAttributesOem(third, FILE_ATTRIBUTE_READONLY));
    flags = GetFileAttributesOem(third);
    CHECK(flags != INVALID_FILE_ATTRIBUTES && (flags & FILE_ATTRIBUTE_READONLY));
    CHECK(SetFileAttributesOem(third, FILE_ATTRIBUTE_NORMAL));
    file = CreateFileOem(first, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
    CHECK(file != INVALID_HANDLE_VALUE && CloseHandle(file));
    find = FindFirstFileOem("*.dat", &data);
    CHECK(find != INVALID_HANDLE_VALUE);
    count = 0;
    do {
        CHECK(!strcmp(data.cFileName, first) || !strcmp(data.cFileName, third));
        ++count;
    } while (FindNextFileOem(find, &data));
    CHECK(GetLastError() == ERROR_NO_MORE_FILES && count == 2 && FindClose(find));
    CHECK(SearchPathOem(".", third, NULL, sizeof(path), path, &part) > 0);
    CHECK(part && !strcmp(part, third));
    CHECK(GetShortPathNameOem(path, short_path, sizeof(short_path)) > 0);
    CHECK(GetFileAttributesOem(short_path) != INVALID_FILE_ATTRIBUTES);
    CHECK(DeleteFileOem(first) && DeleteFileOem(third));
    SetLastError(0);
    CHECK(!DeleteFileOem(third) && GetLastError() == ERROR_FILE_NOT_FOUND);
    puts("S37_OEM_ENUM_SEARCH_RENAME_DELETE_DIRECTORY_OK");

    CHECK(SetEnvironmentVariableOem("TEMP", cwd) && SetEnvironmentVariableOem("TMP", cwd));
    CHECK(GetTempPathOem(sizeof(path), path) > 0);
    CHECK(GetTempFileNameOem(".", "OEM", 0, temp));
    CHECK(GetFileAttributesOem(temp) != INVALID_FILE_ATTRIBUTES && DeleteFileOem(temp));
    CHECK(GetSystemDirectoryOem(path, sizeof(path)) > 0);
    CHECK(GetWindowsDirectoryOem(path, sizeof(path)) > 0);
    memcpy(root, cwd, 3); root[3] = 0;
    CHECK(GetDriveTypeOem(root) == GetDriveTypeA(root));
    CHECK(GetDiskFreeSpaceOem(root, &sectors, &bytes, &free_clusters, &total));
    CHECK(sectors && bytes && total && free_clusters <= total);
    CHECK(GetVolumeInformationOem(root, volume, sizeof(volume), &serial,
        &max_component, &flags, fs, sizeof(fs)) && fs[0] && max_component);
    n = sizeof(computer);
    CHECK(GetComputerNameOem(computer, &n) && n && n == strlen(computer));
    OutputDebugStringOem("S37 OEM debug conversion witness");
    CHECK(!RemoveFontResourceOem("S37-NO-SUCH-FONT.FON"));
    puts("S37_OEM_TEMP_SYSTEM_VOLUME_COMPUTER_NEGATIVE_FONT_OK");
    CHECK(SetCurrentDirectoryW(original));
    CHECK(RemoveDirectoryOem("O37CASE"));
    return 0;
}
