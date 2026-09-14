/* Formal Console membership mechanism; not a broker identity provider. */
#include <windows.h>
#include "broker/console_membership.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static HANDLE ready;

static BOOL child(char *image, char *event, DWORD flags, PROCESS_INFORMATION *p)
{
    STARTUPINFOA startup = {sizeof(startup)};
    char command[2048];
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    if (sprintf_s(command, sizeof(command), "\"%s\" wait %s", image, event) < 0) return FALSE;
    ResetEvent(ready);
    if (!CreateProcessA(image, command, NULL, NULL, FALSE, flags,
        NULL, NULL, &startup, p)) return FALSE;
    return WaitForSingleObject(ready, 5000) == WAIT_OBJECT_0;
}

int main(int argc, char **argv)
{
    HANDLE stop;
    PROCESS_INFORMATION a = {0}, b = {0}, c = {0};
    char image[MAX_PATH], event[128], readyName[160];
    int result = 1;
    DWORD code, i;
    DWORD candidates[3];
    BYTE members[3] = {7,7,7};
    PROCESS_INFORMATION *children[] = {&a, &b, &c};
    if (argc == 3 && !strcmp(argv[1], "wait")) {
        stop = OpenEventA(SYNCHRONIZE, FALSE, argv[2]);
        if (!stop) return 10;
        sprintf_s(readyName, sizeof(readyName), "%s-ready", argv[2]);
        ready = OpenEventA(EVENT_MODIFY_STATE, FALSE, readyName);
        if (!ready || !SetEvent(ready)) return 12;
        CloseHandle(ready);
        code = WaitForSingleObject(stop, 30000);
        CloseHandle(stop);
        return code == WAIT_OBJECT_0 ? 0 : 11;
    }
    if (!GetModuleFileNameA(NULL, image, sizeof(image))) return 2;
    sprintf_s(event, sizeof(event), "Local\\ntvdm-console-fixture-%lu-%lu",
        GetCurrentProcessId(), GetTickCount());
    stop = CreateEventA(NULL, TRUE, FALSE, event);
    if (!stop || GetLastError() == ERROR_ALREADY_EXISTS) return 3;
    sprintf_s(readyName, sizeof(readyName), "%s-ready", event);
    ready = CreateEventA(NULL, TRUE, FALSE, readyName);
    if (!ready) return 3;
    /* Only this test controller detaches. It never changes a user's process. */
    FreeConsole();
    if (!child(image, event, CREATE_NEW_CONSOLE, &a)) goto done;
    if (!AttachConsole(a.dwProcessId)) goto done;
    if (!child(image, event, 0, &b)) goto done;
    candidates[0]=a.dwProcessId; candidates[1]=b.dwProcessId;
    if (broker_console_membership(a.dwProcessId,candidates,2,members)!=ERROR_ACCESS_DENIED ||
        members[0]!=7 || members[1]!=7) goto done;
    FreeConsole();
    if (!child(image, event, CREATE_NEW_CONSOLE, &c)) goto done;
    candidates[2]=c.dwProcessId;
    if (broker_console_membership(b.dwProcessId,candidates,3,members) ||
        members[0]!=1 || members[1]!=1 || members[2]!=0) goto done;
    if (broker_console_membership(c.dwProcessId,candidates,3,members) ||
        members[0]!=0 || members[1]!=0 || members[2]!=1) goto done;
    /* Successful queries leave this controller detached. Invalid requests
     * neither attach nor publish partial output. */
    if (GetConsoleProcessList(&code,1)) goto done;
    if (broker_console_membership(0,candidates,3,members)!=ERROR_INVALID_PARAMETER ||
        members[0]!=0 || members[1]!=0 || members[2]!=1) goto done;
    result = 0;
done:
    FreeConsole();
    SetEvent(stop);
    for (i = 0; i < 3; ++i) if (children[i]->hProcess) {
        if (WaitForSingleObject(children[i]->hProcess, 5000) != WAIT_OBJECT_0) {
            TerminateProcess(children[i]->hProcess, 99); /* Exact owned child only. */
            WaitForSingleObject(children[i]->hProcess, 5000);
            result = 4;
        }
        if (!GetExitCodeProcess(children[i]->hProcess, &code) || code != 0) result = 5;
        CloseHandle(children[i]->hThread); CloseHandle(children[i]->hProcess);
    }
    CloseHandle(stop);
    CloseHandle(ready);
    if (!result) puts("PASS: formal membership, same/different Console, attached-controller refusal, unchanged failure output, detached return, owned-child normal exit");
    else fprintf(stderr, "FAIL console membership result=%d error=%lu\n", result, GetLastError());
    return result;
}
