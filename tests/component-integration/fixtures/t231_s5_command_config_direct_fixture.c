#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opennt-bop/command/opennt_command_composition.h"

typedef struct fixture_context { uint8_t guest[0x10000]; } fixture_context;

static int guest_read(void *state, uint32_t address, uint8_t *buffer, uint32_t bytes)
{
    fixture_context *context = (fixture_context *)state;
    if (context == NULL || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(buffer, context->guest + address, bytes);
    return 1;
}

static int guest_write(void *state, uint32_t address, const uint8_t *buffer, uint32_t bytes)
{
    fixture_context *context = (fixture_context *)state;
    if (context == NULL || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(context->guest + address, buffer, bytes);
    return 1;
}

static void initialize_event(runtime_exception_event *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    event->abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->vector = 6u;
    event->fault_rip = 0x500u;
}

static int invoke(fixture_context *context, runtime_exception_event *event,
    runtime_cpu_state *cpu, runtime_cpu_result *result,
    runtime_command_misc_session *session, uint32_t service)
{
    runtime_command_misc_call call;
    memset(&call, 0, sizeof(call));
    call.magic = RUNTIME_COMMAND_MISC_CALL_MAGIC;
    call.abi_version = RUNTIME_COMMAND_MISC_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.service = service;
    call.boundary = event;
    call.cpu = cpu;
    call.result = result;
    call.guest_state = context;
    call.guest_read = guest_read;
    call.guest_write = guest_write;
    call.session = session;
    return runtime_command_misc_invoke(&call);
}

static int write_text_file(const CHAR *path, const CHAR *text)
{
    HANDLE file;
    DWORD written;
    file = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (file == INVALID_HANDLE_VALUE) return 0;
    if (!WriteFile(file, text, (DWORD)strlen(text), &written, NULL) ||
        written != (DWORD)strlen(text)) { CloseHandle(file); return 0; }
    CloseHandle(file);
    return 1;
}

static int read_text_file(const CHAR *path, CHAR *text, DWORD bytes)
{
    HANDLE file;
    DWORD read;
    file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (file == INVALID_HANDLE_VALUE || bytes < 2u) return 0;
    if (!ReadFile(file, text, bytes - 1u, &read, NULL)) { CloseHandle(file); return 0; }
    CloseHandle(file);
    text[read] = '\0';
    return 1;
}

static int multisz_contains(const CHAR *strings, const CHAR *needle)
{
    if (strings == NULL) return 0;
    while (*strings != '\0') {
        if (strcmp(strings, needle) == 0) return 1;
        strings += strlen(strings) + 1u;
    }
    return 0;
}

int main(void)
{
    fixture_context context;
    runtime_exception_event event;
    runtime_cpu_state cpu;
    runtime_cpu_result result;
    runtime_command_misc_session session;
    CHAR directory[MAX_PATH], config_path[MAX_PATH], autoexec_path[MAX_PATH];
    CHAR text[4096];
    CHAR too_long[65u];

    if (!GetTempPathA(MAX_PATH, directory) ||
        !GetTempFileNameA(directory, "cfc", 0u, config_path) ||
        !GetTempFileNameA(directory, "cfa", 0u, autoexec_path)) return 1;
    if (!write_text_file(config_path,
            "country=999,437,C:\\bad.sys\r\n"
            "device=keep.sys\r\n"
            "shell=%SystemRoot%\\System32\\command.com /e:512\r\n") ||
        !write_text_file(autoexec_path,
            "PROMPT $P$G\r\nPATH=C:\\TOOLS\r\nSET FOO=BAR\r\n")) return 2;

    memset(&context, 0, sizeof(context));
    initialize_event(&event);
    runtime_cpu_state_initialize(&cpu, RUNTIME_CPU_EXECUTION_REAL);
    runtime_command_misc_session_initialize(&session);
    runtime_command_config_set_inputs(&session, config_path, autoexec_path);
    memset(too_long, 'X', sizeof(too_long) - 1u);
    too_long[sizeof(too_long) - 1u] = '\0';
    if (runtime_command_config_set_bootstrap_command(&session, too_long) ||
        runtime_command_config_set_bootstrap_command(&session,
            "O:\\RUNNER\\DOS ROOT\\COMMAND.COM")) return 2;
    if (!runtime_command_config_set_bootstrap_command(&session,
            "O:\\RUNNER\\DOS\\COMMAND.COM")) return 2;
    cpu.ds = 0x100u;
    cpu.edx = 0x100u;

    if (!invoke(&context, &event, &cpu, &result, &session,
            RUNTIME_COMMAND_MISC_GET_CONFIG_SYS) ||
        result.disposition != RUNTIME_CPU_RESULT_RESUME ||
        !read_text_file((CHAR *)context.guest + 0x1100u, text, sizeof(text)) ||
        strstr(text, "country=999") != NULL || strstr(text, "device=keep.sys") == NULL ||
        strstr(text, "shell=O:\\RUNNER\\DOS\\COMMAND.COM /p") == NULL ||
        strstr(text, "/e:512") == NULL) return 3;

    memset(context.guest + 0x1100u, 0, 64u);
    if (!invoke(&context, &event, &cpu, &result, &session,
            RUNTIME_COMMAND_MISC_GET_AUTOEXEC_BAT) ||
        result.disposition != RUNTIME_CPU_RESULT_RESUME ||
        !read_text_file((CHAR *)context.guest + 0x1100u, text, sizeof(text)) ||
        strstr(text, "PROMPT $P$G") == NULL || strstr(text, "PATH=C:\\TOOLS") == NULL ||
        strstr(text, "SET FOO=BAR") == NULL || lpszzcmdEnv16 == NULL ||
        !multisz_contains(lpszzcmdEnv16, "PROMPT") ||
        !multisz_contains(lpszzcmdEnv16, "FOO")) return 4;

    runtime_command_config_set_inputs(&session, "Z:\\does-not-exist\\config.nt", autoexec_path);
    if (!invoke(&context, &event, &cpu, &result, &session,
            RUNTIME_COMMAND_MISC_GET_CONFIG_SYS) ||
        result.disposition != RUNTIME_CPU_RESULT_STOP) return 5;

    DeleteConfigFiles();
    free(lpszzcmdEnv16); lpszzcmdEnv16 = NULL;
    DeleteFileA(config_path); DeleteFileA(autoexec_path);
    puts("T231 S5 direct OpenNT cmdconf config, autoexec, and terminal failure paths verified");
    return 0;
}
