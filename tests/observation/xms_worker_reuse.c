/* Test-only same-Console product reuse witness. No guest patch or substitute. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

static HANDLE find_worker(const char *wanted, DWORD *identity)
{
    DWORD ids[64], count, i;
    HANDLE found = NULL;
    count = GetConsoleProcessList(ids, 64);
    if (!count || count > 64) return NULL;
    for (i = 0; i < count; ++i) {
        HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
            FALSE, ids[i]);
        char path[MAX_PATH]; DWORD length = sizeof(path);
        if (!process) continue;
        if (QueryFullProcessImageNameA(process, 0, path, &length) &&
            !_stricmp(path, wanted)) {
            if (found) { CloseHandle(found); CloseHandle(process); return NULL; }
            found = process;
            *identity = ids[i];
        } else CloseHandle(process);
    }
    return found;
}

int main(int argc, char **argv)
{
    char launcher[MAX_PATH], worker_path[MAX_PATH], command[MAX_PATH + 64];
    FILE *log;
    HANDLE previous = NULL;
    DWORD previous_id = 0, iteration;
    if (argc != 3 || fopen_s(&log, argv[2], "wx")) return 1;
    if (sprintf_s(launcher, sizeof(launcher), "%s\\run16.exe", argv[1]) < 0 ||
        sprintf_s(worker_path, sizeof(worker_path), "%s\\ntvdm.exe", argv[1]) < 0) return 2;
    for (iteration = 0; iteration < 2; ++iteration) {
        STARTUPINFOA startup = {sizeof(startup)};
        PROCESS_INFORMATION child = {0};
        HANDLE worker;
        DWORD code, identity = 0, result;
        sprintf_s(command, sizeof(command), "\"%s\" tests\\X35.COM", launcher);
        if (!CreateProcessA(launcher, command, NULL, NULL, TRUE, 0, NULL,
                argv[1], &startup, &child)) return 3;
        result = WaitForSingleObject(child.hProcess, 20000);
        if (result != WAIT_OBJECT_0) {
            fprintf(log, "iteration=%lu launcher=%lu timeout-or-wait-error=%lu\n",
                iteration, child.dwProcessId, result);
            fflush(log);
            return 4; /* preserve product process for outside diagnosis */
        }
        GetExitCodeProcess(child.hProcess, &code);
        CloseHandle(child.hThread);
        CloseHandle(child.hProcess);
        if (code) { fprintf(log, "launcher-exit=%lu\n", code); fclose(log); return 5; }
        worker = find_worker(worker_path, &identity);
        fprintf(log, "iteration=%lu launcher=%lu worker=%lu\n",
            iteration, child.dwProcessId, identity);
        fflush(log);
        if (!worker) return 6;
        if (previous && (identity != previous_id ||
                WaitForSingleObject(previous, 0) != WAIT_TIMEOUT)) return 7;
        if (previous) CloseHandle(previous);
        previous = worker;
        previous_id = identity;
    }
    CloseHandle(previous);
    puts("S35_SAME_LIVE_WORKER_REUSED_OK");
    fputs("S35_SAME_LIVE_WORKER_REUSED_OK\n", log);
    fclose(log);
    return 0;
}
