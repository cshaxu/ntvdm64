/* Test-only COMSPEC: signal readiness, then require one VM-specific key. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

int main(void)
{
    char role[2], root[MAX_PATH], path[MAX_PATH];
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE), marker;
    DWORD mode, count, root_length;
    BOOL read_ok, restored;
    int path_length;
    WCHAR key;
    root_length = GetEnvironmentVariableA("VM_READY_DIR", root, sizeof(root));
    if (GetEnvironmentVariableA("ROLE", role, sizeof(role)) != 1 ||
        (role[0] != 'A' && role[0] != 'B') ||
        !root_length || root_length >= sizeof(root)) return 40;
    path_length = snprintf(path, sizeof(path), "%s\\READY-%c.BIN", root, role[0]);
    if (path_length < 0 || (size_t)path_length >= sizeof(path) ||
        !GetConsoleMode(input, &mode) ||
        !SetConsoleMode(input, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)))
        return 40;
    marker = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_NEW,
                         FILE_ATTRIBUTE_NORMAL, NULL);
    if (marker == INVALID_HANDLE_VALUE) {
        SetConsoleMode(input, mode);
        return 41;
    }
    CloseHandle(marker);
    read_ok = ReadConsoleW(input, &key, 1, &count, NULL);
    restored = SetConsoleMode(input, mode);
    if (!read_ok || count != 1 || !restored) return 42;
    path_length = snprintf(path, sizeof(path), "%s\\KEY-%c.BIN", root, role[0]);
    if (path_length < 0 || (size_t)path_length >= sizeof(path)) return 45;
    marker = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_NEW,
                         FILE_ATTRIBUTE_NORMAL, NULL);
    if (marker == INVALID_HANDLE_VALUE) return 45;
    if (!WriteFile(marker, &key, sizeof(key), &count, NULL) || count != sizeof(key)) {
        CloseHandle(marker);
        return 45;
    }
    CloseHandle(marker);
#ifdef REVERSE_EXPECTATION
    return key == (role[0] == 'A' ? L'b' : L'a') ? 43 : 44;
#else
    return key == (WCHAR)(role[0] + ('a' - 'A')) ? 43 : 44;
#endif
}
