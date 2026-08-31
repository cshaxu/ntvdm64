/*
 * Fixed, non-debug startup observation container for the selected x86
 * SoftPC product.  It owns a real CONIN$/CONOUT$ console, launches exactly
 * one product command line, waits a bounded interval, and records only the
 * observable process result.  It never attaches a debugger, searches guest
 * memory, installs breakpoints, or changes product inputs.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#define OBSERVATION_TIMEOUT_MS 8000u
#define OBSERVATION_TIMEOUT_EXIT 0x53504354u

static void write_console_snapshot(HANDLE output, const char *report_path)
{
    char screen[4097];
    char path[MAX_PATH];
    DWORD count = 0;
    FILE *file = NULL;

    if (!ReadConsoleOutputCharacterA(output, screen, 4096,
                                     (COORD){ 0, 0 }, &count)) return;
    screen[count] = '\0';
    snprintf(path, sizeof(path), "%s.console.txt", report_path);
    if (fopen_s(&file, path, "wb") == 0 && file != NULL) {
        fwrite(screen, 1, count, file);
        fclose(file);
    }
}

static void clear_console(HANDLE output)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD written;
    DWORD cells;

    if (!GetConsoleScreenBufferInfo(output, &info)) return;
    cells = (DWORD)info.dwSize.X * (DWORD)info.dwSize.Y;
    (void)FillConsoleOutputCharacterA(output, ' ', cells, (COORD){ 0, 0 }, &written);
    (void)FillConsoleOutputAttribute(output, info.wAttributes, cells,
                                     (COORD){ 0, 0 }, &written);
    (void)SetConsoleCursorPosition(output, (COORD){ 0, 0 });
}

int main(int argc, char **argv)
{
    STARTUPINFOA startup = { sizeof(startup) };
    PROCESS_INFORMATION child = { 0 };
    SECURITY_ATTRIBUTES inherit = { sizeof(inherit), NULL, TRUE };
    HANDLE input;
    HANDLE output;
    DWORD wait_status;
    DWORD exit_code = STILL_ACTIVE;
    char command_line[MAX_PATH * 2];
    FILE *report = NULL;

    if (argc != 4) return 64;
    if (!AllocConsole() && GetLastError() != ERROR_ACCESS_DENIED) return 65;
    input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                        OPEN_EXISTING, 0, NULL);
    output = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                         OPEN_EXISTING, 0, NULL);
    if (input == INVALID_HANDLE_VALUE || output == INVALID_HANDLE_VALUE) return 66;

    clear_console(output);
    SetStdHandle(STD_INPUT_HANDLE, input);
    SetStdHandle(STD_OUTPUT_HANDLE, output);
    SetStdHandle(STD_ERROR_HANDLE, output);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = input;
    startup.hStdOutput = output;
    startup.hStdError = output;
    snprintf(command_line, sizeof(command_line), "\"%s\" -f -o --ordinary-child", argv[1]);

    if (!CreateProcessA(NULL, command_line, NULL, NULL, TRUE, 0, NULL, argv[2],
                        &startup, &child)) {
        CloseHandle(input);
        CloseHandle(output);
        return 67;
    }
    CloseHandle(child.hThread);
    wait_status = WaitForSingleObject(child.hProcess, OBSERVATION_TIMEOUT_MS);
    if (wait_status == WAIT_TIMEOUT) {
        TerminateProcess(child.hProcess, OBSERVATION_TIMEOUT_EXIT);
        WaitForSingleObject(child.hProcess, 1000);
    }
    GetExitCodeProcess(child.hProcess, &exit_code);
    write_console_snapshot(output, argv[3]);
    if (fopen_s(&report, argv[3], "wb") == 0 && report != NULL) {
        fprintf(report, "container=console-owning-nondebug\n");
        fprintf(report, "pid=%lu\n", (unsigned long)child.dwProcessId);
        fprintf(report, "result=%s\n", wait_status == WAIT_TIMEOUT ? "timeout" : "exited");
        fprintf(report, "exit=0x%08lx\n", (unsigned long)exit_code);
        fprintf(report, "timeout-ms=%u\n", OBSERVATION_TIMEOUT_MS);
        fclose(report);
    }
    CloseHandle(child.hProcess);
    CloseHandle(input);
    CloseHandle(output);
    return 0;
}
