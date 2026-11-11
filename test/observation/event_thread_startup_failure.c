/* Test-only bounded reproducer. Link the actual product shutdown helper.
 * Process isolation allows the parent to reclaim a deliberately hung child. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "mvdm_softpc_event_thread.h"
__declspec(dllimport) LONG NTAPI NtDelayExecution(BOOLEAN, LARGE_INTEGER *);

static DWORD WINAPI waiter(LPVOID unused)
{
    if (unused) SetEvent((HANDLE)unused);
    LARGE_INTEGER timeout;
    timeout.QuadPart = -100000000;
    return NtDelayExecution(TRUE, &timeout) == 0x101 ? 0 : 13;
}

int main(int argc, char **argv)
{
    if (argc == 2 && (!strcmp(argv[1], "--suspended-child") ||
            !strcmp(argv[1], "--running-child"))) {
        HANDLE ready = CreateEvent(NULL, TRUE, FALSE, NULL);
        HANDLE thread = CreateThread(NULL, 0, waiter, ready,
            !strcmp(argv[1], "--suspended-child") ? CREATE_SUSPENDED : 0, NULL);
        if (!ready || !thread) return 10;
        if (!strcmp(argv[1], "--running-child") &&
            WaitForSingleObject(ready, 1000) != WAIT_OBJECT_0) return 12;
        if (!mvdm_softpc_event_thread_alert_and_join(thread,
                !strcmp(argv[1], "--running-child"))) return 11;
        if (!strcmp(argv[1], "--suspended-child") &&
                WaitForSingleObject(ready, 0) != WAIT_TIMEOUT) return 14;
        if (!strcmp(argv[1], "--running-child")) {
            DWORD code;
            if (!GetExitCodeThread(thread, &code) || code) return 15;
        }
        CloseHandle(thread);
        CloseHandle(ready);
        return 0;
    } else {
        char path[MAX_PATH], command[MAX_PATH + 64];
        STARTUPINFOA startup = {sizeof(startup)};
        PROCESS_INFORMATION child = {0};
        DWORD result, code;
        if (!GetModuleFileNameA(NULL, path, sizeof(path))) return 1;
        if (sprintf_s(command, sizeof(command), "\"%s\" %s", path,
                argc == 2 && !strcmp(argv[1], "--running") ?
                "--running-child" : "--suspended-child") < 0) return 2;
        if (!CreateProcessA(path, command, NULL, NULL, FALSE, CREATE_NO_WINDOW,
                NULL, NULL, &startup, &child)) return 3;
        result = WaitForSingleObject(child.hProcess, 2000);
        if (result == WAIT_TIMEOUT) {
            /* Only this explicitly created disposable fixture is terminated. */
            if (!TerminateProcess(child.hProcess, 99) ||
                WaitForSingleObject(child.hProcess, 5000) != WAIT_OBJECT_0) return 4;
            puts(argc == 2 && !strcmp(argv[1], "--running") ?
                "S35_RUNNING_SLEEP_EX_JOIN_HANG_REPRODUCED" :
                "S35_SUSPENDED_THREAD_JOIN_HANG_REPRODUCED");
            code = 6; /* A timeout now fails the repaired-contract gate. */
        } else {
            if (result != WAIT_OBJECT_0 || !GetExitCodeProcess(child.hProcess, &code)) return 5;
            printf("S35_CHILD_RETURNED_%lu\n", code);
        }
        CloseHandle(child.hThread);
        CloseHandle(child.hProcess);
        return (int)code;
    }
}
