/* T406 real-console integration observer. No debugger, environment selectors,
 * guest-memory access, product flags or functional replacement providers. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

static FILE *report;
static DWORD child_pid;

static BOOL CALLBACK record_control(HWND window, LPARAM unused)
{
    char text[512];
    (void)unused;
    if (GetWindowTextA(window, text, sizeof(text)))
        fprintf(report, "window-text: %s\n", text);
    return TRUE;
}

static BOOL CALLBACK record_window(HWND window, LPARAM unused)
{
    DWORD pid;
    (void)unused;
    GetWindowThreadProcessId(window, &pid);
    if (pid == child_pid) {
        record_control(window, 0);
        EnumChildWindows(window, record_control, 0);
    }
    return TRUE;
}

static void snapshot(HANDLE output, const char *phase)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD mode;
    SHORT row;
    fprintf(report, "\nphase=%s\n", phase);
    if (GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode))
        fprintf(report, "console-input-mode=%08lx\n", mode);
    if (GetConsoleScreenBufferInfo(output, &info)) {
        fprintf(report, "console=%dx%d cursor=%d,%d\n", info.dwSize.X,
            info.dwSize.Y, info.dwCursorPosition.X, info.dwCursorPosition.Y);
        for (row = info.srWindow.Top; row <= info.srWindow.Bottom &&
             row < info.srWindow.Top + 60; ++row) {
            char line[241];
            DWORD count = 0;
            COORD at = {0, row};
            DWORD width = info.dwSize.X < 240 ? info.dwSize.X : 240;
            if (ReadConsoleOutputCharacterA(output, line, width, at, &count)) {
                while (count && line[count - 1] == ' ') --count;
                line[count] = 0;
                fprintf(report, "%s\n", line);
            }
        }
    } else fprintf(report, "console-read-error=%lu\n", GetLastError());
    EnumWindows(record_window, 0);
    fflush(report);
}

static void key(HANDLE input, WORD vk, char ascii)
{
    INPUT_RECORD event = {0};
    DWORD written;
    event.EventType = KEY_EVENT;
    event.Event.KeyEvent.bKeyDown = TRUE;
    event.Event.KeyEvent.wRepeatCount = 1;
    event.Event.KeyEvent.wVirtualKeyCode = vk;
    event.Event.KeyEvent.wVirtualScanCode = (WORD)MapVirtualKeyA(vk, MAPVK_VK_TO_VSC);
    event.Event.KeyEvent.uChar.AsciiChar = ascii;
    WriteConsoleInputA(input, &event, 1, &written);
    Sleep(100);
    event.Event.KeyEvent.bKeyDown = FALSE;
    WriteConsoleInputA(input, &event, 1, &written);
    Sleep(100);
}

int main(int argc, char **argv)
{
    SECURITY_ATTRIBUTES inherit = {sizeof(inherit), NULL, TRUE};
    STARTUPINFOA startup = {0};
    PROCESS_INFORMATION child = {0};
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {0};
    JOBOBJECT_ASSOCIATE_COMPLETION_PORT association = {0};
    HANDLE input, output, job, completion;
    char command[512];
    DWORD result, exit_code = STILL_ACTIVE;
    int timed_out;
    const char *binary;
    const char *package_root = "O:\\ntvdm64";
    char executable[256];

    if (argc != 3 && argc != 4) return 64;
    if (argc == 4) {
        if (strcmp(argv[3], "Q:\\")) return 64;
        package_root = argv[3];
    }
    if (!strcmp(argv[1], "mem")) binary = "mem.exe";
    else if (!strcmp(argv[1], "command")) binary = "command.com";
    else if (!strcmp(argv[1], "edit")) binary = "edit.com";
    else if (!strcmp(argv[1], "write")) binary = "system32\\write.exe";
    else if (!strcmp(argv[1], "ems")) binary = "EMSPROBE.COM";
    else return 64;
    if (strncmp(argv[2], "O:\\ntvdm64\\logs\\", 16)) return 64;
    if (fopen_s(&report, argv[2], "wx") || !report) return 65;
    /* Do not reuse the tool host's pseudo-console or its input queue. */
    FreeConsole();
    if (!AllocConsole()) return 66;
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit, OPEN_EXISTING, 0, NULL);
    output = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit, OPEN_EXISTING, 0, NULL);
    if (input == INVALID_HANDLE_VALUE || output == INVALID_HANDLE_VALUE) return 66;
    SetStdHandle(STD_INPUT_HANDLE, input);
    SetStdHandle(STD_OUTPUT_HANDLE, output);
    SetStdHandle(STD_ERROR_HANDLE, output);
    {
        CONSOLE_SCREEN_BUFFER_INFO initial;
        COORD buffer;
        SMALL_RECT window = {0, 0, 79, 24};
        if (!GetConsoleScreenBufferInfo(output, &initial)) return 66;
        buffer = initial.dwSize;
        if (buffer.X < 80) buffer.X = 80;
        if (!SetConsoleScreenBufferSize(output, buffer) ||
            !SetConsoleWindowInfo(output, TRUE, &window)) {
            fprintf(report, "console-size-error=%lu\n", GetLastError());
            /* Retain and report actual dimensions; this limits UI-layout
             * acceptance, not whether the ordinary workload can execute. */
        }
    }
    FlushConsoleInputBuffer(input);
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = input;
    startup.hStdOutput = startup.hStdError = output;
    job = CreateJobObjectA(NULL, NULL);
    completion = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
    association.CompletionKey = job;
    association.CompletionPort = completion;
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!job || !completion ||
        !SetInformationJobObject(job, JobObjectAssociateCompletionPortInformation,
            &association, sizeof(association)) ||
        !SetInformationJobObject(job, JobObjectExtendedLimitInformation,
            &limits, sizeof(limits))) return 67;
    snprintf(executable, sizeof(executable), "%s\\ntvdm32.exe", package_root);
    snprintf(command, sizeof(command), "%s %s", executable, binary);
    fprintf(report, "command=%s\n", command);
    if (!CreateProcessA(executable, command, NULL, NULL, TRUE,
            CREATE_SUSPENDED, NULL, package_root, &startup, &child)) {
        fprintf(report, "create-error=%lu\n", GetLastError());
        fclose(report);
        return 68;
    }
    if (!AssignProcessToJobObject(job, child.hProcess)) {
        TerminateProcess(child.hProcess, 69);
        return 69;
    }
    child_pid = child.dwProcessId;
    fprintf(report, "pid=%lu\n", child_pid);
    fflush(report);
    ResumeThread(child.hThread);
    CloseHandle(child.hThread);
    result = WaitForSingleObject(child.hProcess, 5000);
    snapshot(output, "first-5s-or-exit");
    if (result == WAIT_TIMEOUT) {
        if (!strcmp(argv[1], "edit")) {
            key(input, VK_ESCAPE, 27);
            fprintf(report, "input=escape-down-up\n");
        } else if (!strcmp(argv[1], "command")) {
            const char *s;
            for (s = "exit"; *s; ++s) key(input, (WORD)(*s - 'a' + 'A'), *s);
            key(input, VK_RETURN, '\r');
            fprintf(report, "input=exit-enter-down-up\n");
        }
        result = WaitForSingleObject(child.hProcess, 15000);
        snapshot(output, "final-20s-or-exit");
    }
    timed_out = result == WAIT_TIMEOUT;
    GetExitCodeProcess(child.hProcess, &exit_code);
    fprintf(report, "wait=%lu timeout=%d exit-before-cleanup=%08lx\n",
        result, timed_out, exit_code);
    {
        DWORD message;
        ULONG_PTR completion_key;
        LPOVERLAPPED process_id;
        while (GetQueuedCompletionStatus(completion, &message, &completion_key,
                &process_id, 0)) {
            if (message == JOB_OBJECT_MSG_NEW_PROCESS)
                fprintf(report, "job-process-pid=%lu\n", (DWORD)(ULONG_PTR)process_id);
        }
    }
    /* Closing this private job also cleans only descendants of our child. */
    CloseHandle(job);
    WaitForSingleObject(child.hProcess, 5000);
    GetExitCodeProcess(child.hProcess, &exit_code);
    fprintf(report, "exit-after-cleanup=%08lx\n", exit_code);
    fclose(report);
    CloseHandle(child.hProcess);
    CloseHandle(completion);
    CloseHandle(input);
    CloseHandle(output);
    return result == WAIT_FAILED ? 70 : 0;
}
