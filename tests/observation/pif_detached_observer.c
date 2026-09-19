/* Test-only no-console launcher. Observe only descendants in our private job.
 * No guest mutation, debugger attachment or product startup switch. */
#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef struct { FILE *report; DWORD worker; } WINDOW_REPORT;
static BOOL CALLBACK child_text(HWND window, LPARAM context)
{
    char text[1024];
    WINDOW_REPORT *state=(WINDOW_REPORT *)context;
    if (GetWindowTextA(window,text,sizeof(text)))
        fprintf(state->report,"window-text=%s\n",text);
    return TRUE;
}
static BOOL CALLBACK worker_window(HWND window, LPARAM context)
{
    DWORD pid=0;
    WINDOW_REPORT *state=(WINDOW_REPORT *)context;
    GetWindowThreadProcessId(window,&pid);
    if (pid==state->worker) {
        child_text(window,context);
        EnumChildWindows(window,child_text,context);
    }
    return TRUE;
}

static void snapshot(FILE *report, HANDLE output)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    CHAR row[512];
    DWORD count;
    SHORT y;
    if (!GetConsoleScreenBufferInfo(output, &info)) return;
    fprintf(report, "buffer=%d,%d cursor=%d,%d\n", info.dwSize.X,
        info.dwSize.Y, info.dwCursorPosition.X, info.dwCursorPosition.Y);
    for (y = 0; y <= info.dwCursorPosition.Y && y < info.dwSize.Y; ++y) {
        COORD position = {0, y};
        DWORD width = info.dwSize.X < 511 ? info.dwSize.X : 511;
        if (!ReadConsoleOutputCharacterA(output, row, width, position, &count)) return;
        while (count && row[count-1] == ' ') --count;
        if (count) { row[count] = 0; fprintf(report, "[%d] %s\n", y, row); }
    }
}
int main(int argc, char **argv)
{
    FILE *report;
    HANDLE job, output = INVALID_HANDLE_VALUE;
    STARTUPINFOA startup = {sizeof(startup)};
    PROCESS_INFORMATION child = {0};
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {0};
    struct { DWORD assigned, listed; ULONG_PTR ids[64]; } members;
    char command[1024];
    DWORD code = STILL_ACTIVE, started, wait = WAIT_TIMEOUT;
    int attached = 0;
    WINDOW_REPORT windows={0};
    if (argc != 3 || strncmp(argv[1], "O:\\winnt\\logs\\", 14) ||
        snprintf(command, sizeof(command), "\"O:\\winnt\\run16.exe\" \"%s\"", argv[2]) >= sizeof(command)) return 64;
    if (fopen_s(&report, argv[1], "wx") || !report) return 65;
    job = CreateJobObjectA(NULL, NULL);
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    startup.dwFlags = STARTF_USESHOWWINDOW; startup.wShowWindow = SW_HIDE;
    if (!job || !SetInformationJobObject(job, JobObjectExtendedLimitInformation,
            &limits, sizeof(limits)) || !CreateProcessA("O:\\winnt\\run16.exe",
            command, NULL, NULL, FALSE, DETACHED_PROCESS | CREATE_SUSPENDED,
            NULL, "O:\\winnt", &startup, &child)) {
        fprintf(report, "create-error=%lu\n", GetLastError()); goto done;
    }
    if (!AssignProcessToJobObject(job, child.hProcess)) {
        fprintf(report, "job-error=%lu\n", GetLastError());
        TerminateProcess(child.hProcess, 66); goto done;
    }
    fprintf(report, "launcher=%lu detached=1\n", child.dwProcessId); fflush(report);
    ResumeThread(child.hThread);
    started = GetTickCount();
    while (GetTickCount() - started < 20000) {
        if (!attached && QueryInformationJobObject(job, JobObjectBasicProcessIdList,
                &members, sizeof(members), NULL)) {
            DWORD i;
            for (i = 0; i < members.listed && i < 64; ++i) {
                HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD)members.ids[i]);
                CHAR image[MAX_PATH]; DWORD length = MAX_PATH;
                if (process && QueryFullProcessImageNameA(process, 0, image, &length) &&
                    !_stricmp(image, "O:\\winnt\\ntvdm.exe")) {
                    FreeConsole();
                    if (AttachConsole((DWORD)members.ids[i])) {
                        attached = 1;
                        windows.worker=(DWORD)members.ids[i];
                        ShowWindow(GetConsoleWindow(), SW_HIDE);
                        output = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                        fprintf(report, "worker=%lu console-attached=1\n", (DWORD)members.ids[i]);
                    }
                }
                if (process) CloseHandle(process);
                if (attached) break;
            }
        }
        wait = WaitForSingleObject(child.hProcess, 100);
        if (wait != WAIT_TIMEOUT) break;
    }
    GetExitCodeProcess(child.hProcess, &code);
    fprintf(report, "wait=%lu exit=%lu\n", wait, code);
    windows.report=report;
    if (windows.worker) EnumWindows(worker_window,(LPARAM)&windows);
    if (output != INVALID_HANDLE_VALUE) snapshot(report, output);
done:
    if (output != INVALID_HANDLE_VALUE) CloseHandle(output);
    if (attached) FreeConsole();
    if (child.hThread) CloseHandle(child.hThread);
    if (child.hProcess) CloseHandle(child.hProcess);
    if (job) CloseHandle(job);
    fclose(report);
    return wait == WAIT_OBJECT_0 && code == 0 && attached ? 0 : 1;
}
