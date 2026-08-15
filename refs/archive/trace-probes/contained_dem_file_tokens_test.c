#include "contained_dem_file_tokens.h"

#include <string.h>
#include <wchar.h>

static BOOL write_fixture_file(LPCWSTR root) {
    WCHAR path[MAX_PATH];
    HANDLE file;
    DWORD written;
    if (swprintf(path, MAX_PATH, L"%ls\\command.com", root) < 0) return FALSE;
    file = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    if (!WriteFile(file, "owned-command", 13, &written, NULL) || written != 13) {
        CloseHandle(file);
        return FALSE;
    }
    return CloseHandle(file);
}

int main(void) {
    WCHAR temp[MAX_PATH];
    WCHAR root[MAX_PATH] = {0};
    NTDOS64_DEM_FILE_SESSION first = {0};
    NTDOS64_DEM_FILE_SESSION second = {0};
    uint32_t first_token;
    uint32_t second_token;
    uint32_t size;
    uint32_t actual;
    char bytes[16] = {0};
    BOOL ok = FALSE;

    if (!GetTempPathW(MAX_PATH, temp) || !GetTempFileNameW(temp, L"ndt", 0, root) ||
        !DeleteFileW(root) || !CreateDirectoryW(root, NULL) || !write_fixture_file(root)) goto done;
    if (!ntdos64_dem_file_session_init(&first, root) ||
        !ntdos64_dem_file_session_init(&second, root)) goto done;
    if (!ntdos64_dem_file_open_read(&first, L"command.com", &first_token, &size) ||
        size != 13 || first_token == 0 ||
        !ntdos64_dem_file_read(&first, first_token, 0, FALSE, bytes, sizeof(bytes), &actual) ||
        actual != 13 || memcmp(bytes, "owned-command", 13) != 0) goto done;
    if (!ntdos64_dem_file_seek(&first, first_token, 6, FILE_BEGIN, &actual) || actual != 6 ||
        !ntdos64_dem_file_read(&first, first_token, 0, TRUE, bytes, 7, &actual) ||
        actual != 7 || memcmp(bytes, "command", 7) != 0) goto done;
    if (ntdos64_dem_file_read(&second, first_token, 0, TRUE, bytes, 1, &actual) ||
        GetLastError() != ERROR_INVALID_HANDLE ||
        ntdos64_dem_file_open_read(&first, L"..\\escape.txt", &second_token, &size) ||
        GetLastError() != ERROR_ACCESS_DENIED) goto done;
    if (!ntdos64_dem_file_close(&first, first_token, TRUE, 13) ||
        ntdos64_dem_file_read(&first, first_token, 0, TRUE, bytes, 1, &actual) ||
        GetLastError() != ERROR_INVALID_HANDLE ||
        !ntdos64_dem_file_open_read(&second, L"command.com", &second_token, &size) ||
        second_token == first_token) goto done;
    ntdos64_dem_file_session_teardown(&first);
    if (ntdos64_dem_file_read(&first, first_token, 0, TRUE, bytes, 1, &actual) ||
        GetLastError() != ERROR_INVALID_HANDLE) goto done;
    ok = TRUE;
done:
    ntdos64_dem_file_session_teardown(&first);
    ntdos64_dem_file_session_teardown(&second);
    if (root[0]) {
        WCHAR file[MAX_PATH];
        if (swprintf(file, MAX_PATH, L"%ls\\command.com", root) >= 0) DeleteFileW(file);
        RemoveDirectoryW(root);
    }
    return ok ? 0 : 1;
}
