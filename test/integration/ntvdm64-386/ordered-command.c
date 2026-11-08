/* Test-only real native processes: the second exits only after observing the
 * first process terminated. Both must be running before either can finish. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    char name[192], role[4], round[4];
    HANDLE mapping = NULL, first_ready = NULL, second_ready = NULL, first = NULL;
    DWORD *pid = NULL, code = 0;
    int result = 100;
    if ((argc != 3 && argc != 5) || strlen(argv[1]) > 100 ||
        !GetEnvironmentVariableA("ROLE", role, sizeof(role)) ||
        !GetEnvironmentVariableA("ROUND", round, sizeof(round))) return 101;
    if ((role[0] != 'A' && role[0] != 'B') ||
        (argv[2][0] != 'A' && argv[2][0] != 'B')) return 102;
    if (argc == 5) {
        char path[MAX_PATH], temporary[MAX_PATH];
        FILE *ready;
        BOOL roundtrip = !strcmp(argv[3], "visible-roundtrip");
        BOOL visible_resize = roundtrip || !strcmp(argv[3], "visible-resize");
        BOOL resize = visible_resize || !strcmp(argv[3], "resize");
        if (resize) {
            HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
            COORD size = {80, 50};
            SMALL_RECT window = {0, 0, 79, 24};
            CONSOLE_SCREEN_BUFFER_INFO info;
            CHAR_INFO marker = {{0}, 0x2f};
            COORD one = {1, 1}, origin = {0, 0}, caret = {1, 25};
            SMALL_RECT target = {0, 25, 0, 25};
            if (roundtrip && round[0] == '1') {
                size.Y = 25; caret.Y = 12; target.Top = target.Bottom = 12;
            }
            if (visible_resize) {
                CONSOLE_FONT_INFOEX font = {sizeof(font)};
                if (!GetCurrentConsoleFontEx(output, FALSE, &font)) return 108;
                font.dwFontSize.Y = 8;
                if (!SetCurrentConsoleFontEx(output, FALSE, &font)) return 109;
            }
            if (!SetConsoleWindowInfo(output, TRUE, &window) ||
                !SetConsoleScreenBufferSize(output, size) ||
                !GetConsoleScreenBufferInfo(output, &info) ||
                info.dwSize.X != size.X || info.dwSize.Y != size.Y) return 106;
            marker.Char.UnicodeChar = (WCHAR)(role[0] + ('a' - 'A'));
            window.Top = visible_resize ? 0 : 25; window.Bottom = size.Y - 1;
            if (!WriteConsoleOutputW(output, &marker, one, origin, &target) ||
                !SetConsoleCursorPosition(output, caret) ||
                !SetConsoleWindowInfo(output, TRUE, &window) ||
                !GetConsoleScreenBufferInfo(output, &info) ||
                info.srWindow.Top != window.Top || info.srWindow.Bottom != window.Bottom) return 107;
        }
        if ((!resize && strcmp(argv[3], "hold")) ||
            sprintf_s(path, sizeof(path), "%s-%c.txt", argv[4], role[0]) < 0 ||
            sprintf_s(temporary, sizeof(temporary), "%s.tmp", path) < 0 ||
            fopen_s(&ready, temporary, resize ? "w" : "wx")) return 103;
        if (resize) fprintf(ready, "%lu resize round=%s\n", GetCurrentProcessId(), round);
        else fprintf(ready, "%lu\n", GetCurrentProcessId());
        if (fclose(ready) || !(resize ? MoveFileExA(temporary, path, MOVEFILE_REPLACE_EXISTING) : MoveFileA(temporary, path))) return 104;
        /* Real native task remains alive until Console/service teardown. */
        if (!resize) {
            WaitForSingleObject(GetCurrentProcess(), INFINITE);
            return 105;
        }
    }
    sprintf_s(name, sizeof(name), "Local\\%s-%s-pid", argv[1], round);
    mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(DWORD), name);
    if (!mapping) goto done;
    pid = MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DWORD));
    sprintf_s(name, sizeof(name), "Local\\%s-%s-first", argv[1], round);
    first_ready = CreateEventA(NULL, TRUE, FALSE, name);
    sprintf_s(name, sizeof(name), "Local\\%s-%s-second", argv[1], round);
    second_ready = CreateEventA(NULL, TRUE, FALSE, name);
    if (!pid || !first_ready || !second_ready) goto done;
    if (role[0] == argv[2][0]) {
        *pid = GetCurrentProcessId();
        if (!SetEvent(first_ready) || WaitForSingleObject(second_ready, INFINITE) != WAIT_OBJECT_0) goto done;
    } else {
        if (WaitForSingleObject(first_ready, INFINITE) != WAIT_OBJECT_0) goto done;
        first = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, *pid);
        if (!first || !SetEvent(second_ready) || WaitForSingleObject(first, INFINITE) != WAIT_OBJECT_0 ||
            !GetExitCodeProcess(first, &code) || code != (DWORD)(argv[2][0] == 'A' ? 37 : 38)) goto done;
    }
    result = role[0] == 'A' ? 37 : 38;
done:
    if (first) CloseHandle(first);
    if (second_ready) CloseHandle(second_ready);
    if (first_ready) CloseHandle(first_ready);
    if (pid) UnmapViewOfFile(pid);
    if (mapping) CloseHandle(mapping);
    return result;
}
