/*
 * Runs the published product in a real Windows Console without any of the
 * MVDM_* diagnostic environment used by console_startup_observer.  Codex's
 * command runner supplies pipe standard handles, which are not equivalent to
 * an interactive cmd.exe Console; explicitly supplying CONIN$/CONOUT$ makes
 * this a release integration test for the latter contract.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_BYTES 32768u
#define RUN_TIMEOUT_MS 30000u

static const char *const diagnostic_variables[] = {
    "MVDM_EXCEPTION_REPORT_PATH", "MVDM_MAIN_RETURN_REPORT_PATH",
    "MVDM_BOP_RETURN_REPORT_PATH", "MVDM_STREAM_IO_REPORT_PATH",
    "MVDM_BASE_VDM_REPORT_PATH", "MVDM_DEM_OPEN_REPORT_PATH",
    "MVDM_CONFIG_DONE_REPORT_PATH", "MVDM_SAS_STORE_REPORT_PATH",
    "MVDM_SAS_STORE_LINEAR", "MVDM_DEM_READ_REPORT_PATH",
    "MVDM_DEM_SEEK_REPORT_PATH", "MVDM_DEM_IOCTL_REPORT_PATH",
    "MVDM_SESSION_DISPOSE_REPORT_PATH", "MVDM_COMMAND_CONTINUATION_REPORT_PATH",
    "MVDM_NATIVE_CHILD_REPORT_PATH", "MVDM_CONSOLE_PRESENTATION_REPORT_PATH"
};

static BOOL append_quoted(char *line, size_t capacity, size_t *used,
                          const char *argument)
{
    size_t count = strlen(argument);
    if (*used + count + 4u >= capacity) return FALSE;
    if (*used != 0u) line[(*used)++] = ' ';
    line[(*used)++] = '"';
    memcpy(line + *used, argument, count);
    *used += count;
    line[(*used)++] = '"';
    line[*used] = '\0';
    return TRUE;
}

static void clear_console(HANDLE output)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD written;
    COORD origin = { 0, 0 };
    if (!GetConsoleScreenBufferInfo(output, &info)) return;
    FillConsoleOutputCharacterA(output, ' ',
        (DWORD)(info.dwSize.X * info.dwSize.Y), origin, &written);
    FillConsoleOutputAttribute(output, info.wAttributes,
        (DWORD)(info.dwSize.X * info.dwSize.Y), origin, &written);
    SetConsoleCursorPosition(output, origin);
}

static BOOL read_console(HANDLE output, char *text, DWORD capacity)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD count = 0;
    COORD origin = { 0, 0 };
    if (capacity == 0u || !GetConsoleScreenBufferInfo(output, &info)) return FALSE;
    if (!ReadConsoleOutputCharacterA(output, text, capacity - 1u, origin, &count))
        return FALSE;
    text[count] = '\0';
    return TRUE;
}

static void save_and_clear_diagnostics(char values[][MAX_PATH], BOOL present[])
{
    DWORD index;
    for (index = 0u; index < ARRAYSIZE(diagnostic_variables); ++index) {
        DWORD length = GetEnvironmentVariableA(diagnostic_variables[index],
                                               values[index], MAX_PATH);
        present[index] = length != 0u && length < MAX_PATH;
        SetEnvironmentVariableA(diagnostic_variables[index], NULL);
    }
}

static void restore_diagnostics(const char values[][MAX_PATH], const BOOL present[])
{
    DWORD index;
    for (index = 0u; index < ARRAYSIZE(diagnostic_variables); ++index)
        SetEnvironmentVariableA(diagnostic_variables[index],
            present[index] ? values[index] : NULL);
}

static BOOL write_console_input_text(HANDLE input, const char *text)
{
    const char *cursor;
    for (cursor = text; *cursor != '\0'; ++cursor) {
        INPUT_RECORD records[2] = { 0 };
        DWORD written;
        SHORT translated = VkKeyScanA(*cursor);
        WORD virtual_key = translated == -1 ? 0u : (WORD)(translated & 0xff);
        records[0].EventType = KEY_EVENT;
        records[0].Event.KeyEvent.bKeyDown = TRUE;
        records[0].Event.KeyEvent.wRepeatCount = 1u;
        records[0].Event.KeyEvent.wVirtualKeyCode = virtual_key;
        records[0].Event.KeyEvent.wVirtualScanCode = (WORD)MapVirtualKeyA(
            virtual_key, MAPVK_VK_TO_VSC);
        records[0].Event.KeyEvent.uChar.AsciiChar = *cursor;
        records[1] = records[0];
        records[1].Event.KeyEvent.bKeyDown = FALSE;
        if (!WriteConsoleInputA(input, records, ARRAYSIZE(records), &written) ||
            written != ARRAYSIZE(records)) return FALSE;
    }
    return TRUE;
}

static int run_case(const char *product, const char *working_directory,
                    const char *command, const char *input_text,
                    const char *expected, HANDLE input, HANDLE output,
                    char *screen, DWORD screen_capacity)
{
    STARTUPINFOA startup = { sizeof(startup) };
    PROCESS_INFORMATION child = { 0 };
    char command_line[4096];
    char values[ARRAYSIZE(diagnostic_variables)][MAX_PATH];
    BOOL present[ARRAYSIZE(diagnostic_variables)];
    size_t used = 0u;
    DWORD wait_status;
    DWORD exit_code = 0u;

    command_line[0] = '\0';
    if (!append_quoted(command_line, sizeof(command_line), &used, product) ||
        !append_quoted(command_line, sizeof(command_line), &used, command)) return 10;
    clear_console(output);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = input;
    startup.hStdOutput = output;
    startup.hStdError = output;
    save_and_clear_diagnostics(values, present);
    if (!CreateProcessA(NULL, command_line, NULL, NULL, TRUE, 0, NULL,
                        working_directory, &startup, &child)) {
        restore_diagnostics(values, present);
        return 11;
    }
    restore_diagnostics(values, present);
    if (input_text != NULL && !write_console_input_text(input, input_text))
        return 16;
    wait_status = WaitForSingleObject(child.hProcess, RUN_TIMEOUT_MS);
    if (wait_status == WAIT_OBJECT_0) GetExitCodeProcess(child.hProcess, &exit_code);
    CloseHandle(child.hThread);
    CloseHandle(child.hProcess);
    if (wait_status != WAIT_OBJECT_0) return 12;
    if (exit_code != 0u) return 13;
    if (!read_console(output, screen, screen_capacity)) return 14;
    return expected == NULL || strstr(screen, expected) != NULL ? 0 : 15;
}

int main(int argc, char **argv)
{
    SECURITY_ATTRIBUTES inherit = { sizeof(inherit), NULL, TRUE };
    HANDLE input;
    HANDLE output;
    char screen[SCREEN_BYTES];
    FILE *report;
    int first;
    int nested;

    int interactive;
    if (argc != 3) return 64;
    /* A pseudoconsole has no Console window but is already attached; only
     * allocate when CONIN$ proves that there is no Console at all. */
    input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                        OPEN_EXISTING, 0, NULL);
    if (input == INVALID_HANDLE_VALUE) {
        if (!AllocConsole()) return 65;
        input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                            OPEN_EXISTING, 0, NULL);
    }
    output = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                         OPEN_EXISTING, 0, NULL);
    if (input == INVALID_HANDLE_VALUE || output == INVALID_HANDLE_VALUE) return 66;
    first = run_case(argv[1], ".", "command.com /c ver", NULL,
                     "MS-DOS Version 5.00.500", input, output, screen,
                     sizeof(screen));
    nested = run_case(argv[1], ".", "command.com /c command.com /c ver", NULL,
                      "MS-DOS Version 5.00.500", input, output, screen,
                      sizeof(screen));
    interactive = run_case(argv[1], ".", "command.com", "exit\r", NULL,
                           input, output, screen, sizeof(screen));
    if (fopen_s(&report, argv[2], "wb") == 0 && report != NULL) {
        fprintf(report, "container=clean-console-no-mvdm-diagnostics\n");
        fprintf(report, "case=command.com /c ver\nresult=%d\n", first);
        fprintf(report, "case=command.com /c command.com /c ver\nresult=%d\n", nested);
        fprintf(report, "case=command.com [Console exit]\nresult=%d\n", interactive);
        fclose(report);
    }
    CloseHandle(input);
    CloseHandle(output);
    return first != 0 ? first : nested != 0 ? nested : interactive;
}
