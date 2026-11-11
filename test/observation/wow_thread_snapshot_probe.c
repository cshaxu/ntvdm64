/* Noninteractive check that the failure diagnostic resumes its owned child. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#define WOW_PROBE_PREFIX "snapshot-test: "
#include "wow_probe_thread_snapshot.h"

int main(int argc, char **argv)
{
    SECURITY_ATTRIBUTES attributes = { sizeof(attributes), NULL, TRUE };
    STARTUPINFOA startup = { sizeof(startup) };
    PROCESS_INFORMATION child = {0};
    HANDLE event;
    char image[MAX_PATH], command[MAX_PATH + 64];
    DWORD code = 1;
    if (argc == 2) {
        HANDLE inherited = (HANDLE)(ULONG_PTR)strtoul(argv[1], NULL, 16);
        return WaitForSingleObject(inherited, INFINITE) == WAIT_OBJECT_0 ? 0 : 2;
    }
    event = CreateEventA(&attributes, TRUE, FALSE, NULL);
    if (!event || !GetModuleFileNameA(NULL, image, sizeof(image))) return 3;
    if (sprintf_s(command, sizeof(command), "\"%s\" %lx", image,
            (unsigned long)(ULONG_PTR)event) < 0 ||
            !CreateProcessA(image, command, NULL, NULL, TRUE, CREATE_NO_WINDOW,
                NULL, NULL, &startup, &child)) {
        CloseHandle(event); return 4;
    }
    report_stalled_worker(child.dwProcessId);
    if (WaitForSingleObject(child.hProcess, 0) == WAIT_TIMEOUT && SetEvent(event) &&
            WaitForSingleObject(child.hProcess, 10000) == WAIT_OBJECT_0 &&
            GetExitCodeProcess(child.hProcess, &code) && code == 0) {
        puts("PASS snapshot resumes owned child and child exits normally");
    } else {
        TerminateProcess(child.hProcess, 5); code = 5;
    }
    CloseHandle(child.hThread); CloseHandle(child.hProcess); CloseHandle(event);
    return (int)code;
}
