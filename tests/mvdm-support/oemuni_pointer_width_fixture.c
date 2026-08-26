#include <windows.h>
#include <oemuni.h>

#include <stdio.h>
#include <stddef.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "FAIL: %s\n", message);
    return 1;
}

int main(void)
{
    char temporary[MAX_PATH];
    char directory[MAX_PATH];
    char file[MAX_PATH];
    char pattern[MAX_PATH];
    char full[MAX_PATH];
    char search[MAX_PATH];
    char original_directory[MAX_PATH];
    LPSTR file_part;
    WIN32_FIND_DATAA data;
    HANDLE find;
    HANDLE handle;
    DWORD result;

    if (GetTempPathA(MAX_PATH, temporary) == 0) return fail("GetTempPathA");
    if (GetTempFileNameA(temporary, "ovl", 0, file) == 0) return fail("GetTempFileNameA");
    DeleteFileA(file);
    if (!CreateDirectoryA(file, NULL)) return fail("CreateDirectoryA");
    strncpy_s(directory, sizeof(directory), file, _TRUNCATE);
    snprintf(file, sizeof(file), "%s\\alpha.txt", directory);
    handle = CreateFileA(file, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE) return fail("CreateFileA");
    CloseHandle(handle);
    snprintf(pattern, sizeof(pattern), "%s\\*", directory);

    find = FindFirstFileOem(pattern, &data);
    if (find == INVALID_HANDLE_VALUE || data.cFileName[0] == '\0') return fail("FindFirstFileOem");
    if (!FindNextFileOem(find, &data) && GetLastError() != ERROR_NO_MORE_FILES) return fail("FindNextFileOem");
    FindClose(find);

    file_part = NULL;
    result = GetFullPathNameOem(file, MAX_PATH, full, &file_part);
    if (result == 0 || file_part == NULL || file_part < full || file_part >= full + strlen(full)) return fail("GetFullPathNameOem file part");
    if (GetCurrentDirectoryA(MAX_PATH, original_directory) == 0) return fail("GetCurrentDirectoryA");
    if (!SetCurrentDirectoryA(directory)) return fail("SetCurrentDirectoryA");
    file_part = NULL;
    result = SearchPathOem(NULL, "alpha", ".txt", MAX_PATH, search, &file_part);
    SetCurrentDirectoryA(original_directory);
    if (result == 0 || file_part == NULL || file_part < search || file_part >= search + strlen(search)) {
        fprintf(stderr, "SearchPathOem result=%lu error=%lu output=%s offset=%td\n", result, GetLastError(), search,
            file_part == NULL ? (ptrdiff_t)-1 : file_part - search);
        return fail("SearchPathOem file part");
    }

    DeleteFileA(file);
    RemoveDirectoryA(directory);
    puts("PASS: x64 OEM pointer-width overlay contracts");
    return 0;
}
