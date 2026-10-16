/* Ordinary shared Win16 launch receipts promise startup, not task exit.
 * Use the public Run16 --wait contract for both task-completion witnesses. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    PROCESS_INFORMATION child[2] = {{0}};
    STARTUPINFOA start = {0};
    char command[2048], launcher[MAX_PATH], *slash;
    FILE *log = NULL;
    DWORD status, waited;
    unsigned i;
    int result = 1;
    if (argc != 5 || fopen_s(&log, argv[3], "wb")) return 2;
    if (strcpy_s(launcher, sizeof(launcher), argv[4]) ||
        !(slash = strrchr(launcher, '\\'))) { fclose(log); return 2; }
    if (strcpy_s(slash+1, sizeof(launcher)-(size_t)(slash+1-launcher), "run16.exe")) {
        fclose(log); return 2;
    }
    start.cb = sizeof(start);
    for (i = 0; i < 2; ++i) {
        if (sprintf_s(command, sizeof(command), "\"%s\" --wait \"%s\"", launcher, argv[i+1]) < 0 ||
            !CreateProcessA(launcher, command, NULL, NULL, FALSE, 0,
                NULL, NULL, &start, &child[i])) goto done;
        fprintf(log, "launch%u returned_pid=%lu flags=0 command=%s\n", i, child[i].dwProcessId, command);
        fflush(log);
        CloseHandle(child[i].hThread); child[i].hThread = NULL;
        if (!i) {
            char report[MAX_PATH], *extension;
            ULONGLONG deadline = GetTickCount64()+10000;
            WIN32_FILE_ATTRIBUTE_DATA file;
            if (strcpy_s(report, sizeof(report), argv[1]) ||
                !(extension = strrchr(report, '.')) ||
                strcpy_s(extension, sizeof(report)-(size_t)(extension-report), ".BIN")) goto done;
            /* Ordinary native CreateProcess returns before the first VDM is
             * ready. Wait for A's actual guest checkpoint, not a fixed delay. */
            for (;;) {
                if (GetFileAttributesExA(report, GetFileExInfoStandard, &file) &&
                    file.nFileSizeLow >= 22) break;
                if (WaitForSingleObject(child[0].hProcess, 0) != WAIT_TIMEOUT ||
                    GetTickCount64() >= deadline) {
                    status = STILL_ACTIVE;
                    (void)GetExitCodeProcess(child[0].hProcess, &status);
                    fprintf(log, "registrar startup missing exit=%lu\n", status);
                    goto done;
                }
                Sleep(10);
            }
            fputs("registrar guest checkpoint observed\n", log); fflush(log);
        }
    }
    waited = WaitForSingleObject(child[1].hProcess, 15000);
    status = STILL_ACTIVE;
    (void)GetExitCodeProcess(child[1].hProcess, &status);
    fprintf(log, "peer wait=%lu exit=%lu\n", waited, status); fflush(log);
    if (waited != WAIT_OBJECT_0 || status ||
        WaitForSingleObject(child[0].hProcess, 0) != WAIT_TIMEOUT) goto done;
    fputs("peer-exited registrar-still-running\n", log); fflush(log);
    waited = WaitForSingleObject(child[0].hProcess, 15000);
    status = STILL_ACTIVE;
    (void)GetExitCodeProcess(child[0].hProcess, &status);
    fprintf(log, "registrar wait=%lu exit=%lu\n", waited, status);
    if (waited != WAIT_OBJECT_0 || status) goto done;
    result = 0;
done:
    fprintf(log, "result=%d error=%lu\n", result, GetLastError());
    fclose(log);
    for (i = 0; i < 2; ++i) {
        if (child[i].hThread) CloseHandle(child[i].hThread);
        if (child[i].hProcess) CloseHandle(child[i].hProcess);
    }
    return result;
}
