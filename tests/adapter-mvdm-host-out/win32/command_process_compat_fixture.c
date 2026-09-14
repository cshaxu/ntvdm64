#include <stdio.h>
#include <string.h>

#include "command_process_compat.h"

int main(int argc, char **argv)
{
    SECURITY_ATTRIBUTES attributes;
    STARTUPINFOA startup;
    PROCESS_INFORMATION process;
    HANDLE output_read = NULL;
    HANDLE output_write = NULL;
    HANDLE error_read = NULL;
    HANDLE error_write = NULL;
    HANDLE input_read = NULL;
    HANDLE input_write = NULL;
    HANDLE original_input;
    HANDLE original_output;
    HANDLE original_error;
    CHAR command_line[MAX_PATH + 32];
    CHAR comspec[MAX_PATH];
    CHAR output[128];
    DWORD bytes = 0u;
    DWORD exit_code = 0u;

    if (argc == 2 && !strcmp(argv[1], "--launcher-child")) {
        puts("COMMAND-CHILD-STREAM");
        return 0;
    }
    if (GetEnvironmentVariableA("COMSPEC", comspec, sizeof(comspec)) == 0u ||
        _snprintf_s(command_line, sizeof(command_line), _TRUNCATE,
            "\"%s\" /c --launcher-child", comspec) < 0) return 1;
    attributes.nLength = sizeof(attributes);
    attributes.lpSecurityDescriptor = NULL;
    attributes.bInheritHandle = TRUE;
    if (!CreatePipe(&input_read, &input_write, &attributes, 0u) ||
        !CreatePipe(&output_read, &output_write, &attributes, 0u) ||
        !CreatePipe(&error_read, &error_write, &attributes, 0u)) return 1;
    if (!SetHandleInformation(input_write, HANDLE_FLAG_INHERIT, 0u) ||
        !SetHandleInformation(output_read, HANDLE_FLAG_INHERIT, 0u) ||
        !SetHandleInformation(error_read, HANDLE_FLAG_INHERIT, 0u)) return 2;

    original_input = GetStdHandle(STD_INPUT_HANDLE);
    original_output = GetStdHandle(STD_OUTPUT_HANDLE);
    original_error = GetStdHandle(STD_ERROR_HANDLE);
    if (!opennt_command_set_std_handle(STD_INPUT_HANDLE, input_read) ||
        !opennt_command_set_std_handle(STD_OUTPUT_HANDLE, output_write) ||
        !opennt_command_set_std_handle(STD_ERROR_HANDLE, error_write)) return 3;
    ZeroMemory(&startup, sizeof(startup));
    ZeroMemory(&process, sizeof(process));
    startup.cb = sizeof(startup);
    if (!opennt_command_create_process_a(NULL, command_line, NULL, NULL, TRUE,
            CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &startup, &process)) return 4;
    CloseHandle(input_read);
    CloseHandle(output_write);
    CloseHandle(error_write);
    if (WaitForSingleObject(process.hProcess, 5000u) != WAIT_OBJECT_0 ||
        !GetExitCodeProcess(process.hProcess, &exit_code) || exit_code != 0u)
        return 5;
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    ZeroMemory(output, sizeof(output));
    if (!ReadFile(output_read, output, sizeof(output) - 1u, &bytes, NULL) ||
        bytes == 0u || strstr(output, "COMMAND-CHILD-STREAM") == NULL) return 6;
    if (GetStdHandle(STD_INPUT_HANDLE) != original_input ||
        GetStdHandle(STD_OUTPUT_HANDLE) != original_output ||
        GetStdHandle(STD_ERROR_HANDLE) != original_error) return 7;
    CloseHandle(input_write);
    CloseHandle(output_read);
    CloseHandle(error_read);
    puts("PASS: child-only COMMAND standard-stream binding");
    return 0;
}
