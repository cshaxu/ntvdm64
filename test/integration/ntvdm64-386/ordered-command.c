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
        if (strcmp(argv[3], "hold") ||
            sprintf_s(path, sizeof(path), "%s-%c.txt", argv[4], role[0]) < 0 ||
            sprintf_s(temporary, sizeof(temporary), "%s.tmp", path) < 0 ||
            fopen_s(&ready, temporary, "wx")) return 103;
        fprintf(ready, "%lu\n", GetCurrentProcessId());
        if (fclose(ready) || !MoveFileA(temporary, path)) return 104;
        /* Real native task remains alive until Console/service teardown. */
        WaitForSingleObject(GetCurrentProcess(), INFINITE);
        return 105;
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
