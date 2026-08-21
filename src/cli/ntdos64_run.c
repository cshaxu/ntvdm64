#include <windows.h>
#include <shellapi.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "ntdos64_startup_selection.h"
#include "ntdos64_config.h"
#include "byob_target_selection.h"
#include "byob_launch_plan_v2.h"

typedef enum image_kind {
    IMAGE_KIND_UNKNOWN,
    IMAGE_KIND_DOS,
    IMAGE_KIND_COMMAND_INPUT,
    IMAGE_KIND_NE,
    IMAGE_KIND_PE32,
    IMAGE_KIND_PE64
} image_kind;

/* One runner invocation owns at most one child process.  The console handler
 * only signals cancellation; run_process remains the sole owner that stops
 * and reaps that child. */
static HANDLE cancellation_event;

static BOOL WINAPI handle_console_control(DWORD control_type)
{
    if (control_type == CTRL_C_EVENT || control_type == CTRL_BREAK_EVENT ||
        control_type == CTRL_CLOSE_EVENT) {
        if (cancellation_event != NULL) SetEvent(cancellation_event);
        return TRUE;
    }
    return FALSE;
}

static int append_text(wchar_t **cursor, size_t *remaining, const wchar_t *text)
{
    size_t length = wcslen(text);
    if (length + 1u > *remaining) return 0;
    memcpy(*cursor, text, length * sizeof(*text));
    *cursor += length;
    *remaining -= length;
    return 1;
}

static int append_argument(wchar_t **cursor, size_t *remaining, const wchar_t *argument)
{
    const wchar_t *scan = argument;
    int quoted = *argument == L'\0';

    while (*scan != L'\0') {
        if (*scan == L' ' || *scan == L'\t' || *scan == L'"') quoted = 1;
        ++scan;
    }
    if (!quoted) return append_text(cursor, remaining, argument);
    if (*remaining < 2u) return 0;
    *(*cursor)++ = L'"';
    --*remaining;
    while (*argument != L'\0') {
        size_t slashes = 0u;
        while (*argument == L'\\') {
            ++slashes;
            ++argument;
        }
        if (*argument == L'"') {
            while (slashes++ * 2u + 1u != 0u) {
                if (*remaining < 2u) return 0;
                *(*cursor)++ = L'\\';
                --*remaining;
            }
            *(*cursor)++ = L'"';
            --*remaining;
            ++argument;
        } else {
            while (slashes-- != 0u) {
                if (*remaining < 2u) return 0;
                *(*cursor)++ = L'\\';
                --*remaining;
            }
            if (*argument != L'\0') {
                if (*remaining < 2u) return 0;
                *(*cursor)++ = *argument++;
                --*remaining;
            }
        }
    }
    if (*remaining < 2u) return 0;
    *(*cursor)++ = L'"';
    --*remaining;
    return 1;
}

static wchar_t *build_command_line(int argc, wchar_t **argv)
{
    size_t capacity = 2u;
    wchar_t *line;
    wchar_t *cursor;
    size_t remaining;
    int index;

    for (index = 0; index < argc; ++index) capacity += wcslen(argv[index]) * 2u + 4u;
    line = (wchar_t *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        capacity * sizeof(*line));
    if (line == NULL) return NULL;
    cursor = line;
    remaining = capacity;
    for (index = 0; index < argc; ++index) {
        if (index != 0 && !append_text(&cursor, &remaining, L" ")) break;
        if (!append_argument(&cursor, &remaining, argv[index])) break;
    }
    if (index != argc || remaining == 0u) {
        HeapFree(GetProcessHeap(), 0, line);
        return NULL;
    }
    *cursor = L'\0';
    return line;
}

static image_kind classify_image(const wchar_t *path)
{
    const wchar_t *extension;
    HANDLE file;
    uint8_t header[64] = {0};
    DWORD bytes_read = 0u;
    uint32_t offset;
    uint8_t nt_header[26] = {0};
    LARGE_INTEGER file_offset;
    uint16_t optional_magic;

    /* PIF and BAT are COMMAND-owned initial-input forms.  Classify solely by
     * their suffix before opening the file: the CLI must neither parse PIF
     * records nor inspect BAT content, and an MZ-looking byte prefix cannot
     * turn either form into a direct host launch. */
    extension = wcsrchr(path, L'.');
    if (extension != NULL && (_wcsicmp(extension, L".bat") == 0 ||
        _wcsicmp(extension, L".pif") == 0)) return IMAGE_KIND_COMMAND_INPUT;

    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !ReadFile(file, header, sizeof(header),
        &bytes_read, NULL)) {
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        return IMAGE_KIND_UNKNOWN;
    }
    CloseHandle(file);
    if (bytes_read < 2u || header[0] != 'M' || header[1] != 'Z') {
        extension = wcsrchr(path, L'.');
        return extension != NULL && _wcsicmp(extension, L".com") == 0 ?
            IMAGE_KIND_DOS : IMAGE_KIND_UNKNOWN;
    }
    if (bytes_read < sizeof(header)) return IMAGE_KIND_DOS;
    memcpy(&offset, header + 0x3cu, sizeof(offset));
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    file_offset.QuadPart = offset;
    if (file == INVALID_HANDLE_VALUE || !SetFilePointerEx(file, file_offset,
        NULL, FILE_BEGIN) || !ReadFile(file, nt_header, sizeof(nt_header),
        &bytes_read, NULL) || bytes_read < sizeof(nt_header)) {
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        return IMAGE_KIND_DOS;
    }
    CloseHandle(file);
    if (nt_header[0] == 'N' && nt_header[1] == 'E') return IMAGE_KIND_NE;
    if (memcmp(nt_header, "PE\0\0", 4u) == 0) {
        memcpy(&optional_magic, nt_header + 24u, sizeof(optional_magic));
        if (optional_magic == 0x010bu) return IMAGE_KIND_PE32;
        if (optional_magic == 0x020bu) return IMAGE_KIND_PE64;
    }
    return IMAGE_KIND_DOS;
}

static int environment_name_compare(const wchar_t *entry, const wchar_t *name)
{
    while (*entry != L'=' && *entry != L'\0' && *name != L'\0') {
        wint_t entry_char = towupper((wint_t)*entry++);
        wint_t name_char = towupper((wint_t)*name++);
        if (entry_char != name_char) return entry_char < name_char ? -1 : 1;
    }
    if (*entry == L'=') return *name == L'\0' ? 0 : -1;
    return *name == L'\0' ? 0 : 1;
}

/* The adapter runtime is in the engine process, not in this runner. Keep the
 * selected resources immutable to this invocation by constructing a child-only
 * environment. This does not expose guest payloads or add a Bochs option. */
static wchar_t *build_adapter_environment(const wchar_t *profile,
    const wchar_t *root, const wchar_t *include_drives,
    const wchar_t *exclude_drives, const wchar_t *launch_plan,
    const wchar_t *config_source, const wchar_t *autoexec_source,
    const wchar_t *mutation_mode)
{
    /* CreateProcess requires its Unicode environment block sorted by name. */
    static const wchar_t *const names[] = {
        L"NTDOS64_ADAPTER_LAUNCH_PLAN", L"NTDOS64_ADAPTER_PROFILE",
        L"NTDOS64_ADAPTER_ROOT", L"NTDOS64_HOST_EXCLUDE_DRIVES",
        L"NTDOS64_HOST_INCLUDE_DRIVES", L"NTDOS64_MUTATION_MODE",
        L"NTDOS64_STARTUP_AUTOEXEC_SOURCE", L"NTDOS64_STARTUP_CONFIG_SOURCE"
    };
    const wchar_t *values[] = { launch_plan, profile, root, exclude_drives,
        include_drives, mutation_mode,
        autoexec_source != NULL ? autoexec_source : L"",
        config_source != NULL ? config_source : L"" };
    LPWCH inherited;
    const wchar_t *entry;
    size_t capacity = 1u;
    size_t index;
    wchar_t *environment;
    wchar_t *cursor;

    if (profile == NULL || root == NULL || include_drives == NULL || exclude_drives == NULL ||
        launch_plan == NULL || mutation_mode == NULL ||
        *profile == L'\0' || *root == L'\0') return NULL;
    inherited = GetEnvironmentStringsW();
    if (inherited == NULL) return NULL;
    for (entry = inherited; *entry != L'\0'; entry += wcslen(entry) + 1u) {
        int replace = 0;
        for (index = 0u; index < sizeof(names) / sizeof(names[0]); ++index) {
            if (environment_name_compare(entry, names[index]) == 0) replace = 1;
        }
        if (!replace) capacity += wcslen(entry) + 1u;
    }
    for (index = 0u; index < sizeof(names) / sizeof(names[0]); ++index) {
        capacity += wcslen(names[index]) + 1u + wcslen(values[index]) + 1u;
    }
    environment = (wchar_t *)HeapAlloc(GetProcessHeap(), 0u,
        capacity * sizeof(*environment));
    if (environment == NULL) {
        FreeEnvironmentStringsW(inherited);
        return NULL;
    }
    cursor = environment;
    index = 0u;
    for (entry = inherited; *entry != L'\0'; entry += wcslen(entry) + 1u) {
        int replace = 0;
        while (index < sizeof(names) / sizeof(names[0]) &&
            environment_name_compare(entry, names[index]) > 0) {
            size_t name_length = wcslen(names[index]);
            size_t value_length = wcslen(values[index]);
            memcpy(cursor, names[index], name_length * sizeof(*cursor));
            cursor += name_length;
            *cursor++ = L'=';
            memcpy(cursor, values[index], value_length * sizeof(*cursor));
            cursor += value_length;
            *cursor++ = L'\0';
            ++index;
        }
        for (size_t candidate = 0u;
            candidate < sizeof(names) / sizeof(names[0]); ++candidate) {
            if (environment_name_compare(entry, names[candidate]) == 0) replace = 1;
        }
        if (!replace) {
            size_t length = wcslen(entry) + 1u;
            memcpy(cursor, entry, length * sizeof(*cursor));
            cursor += length;
        }
    }
    while (index < sizeof(names) / sizeof(names[0])) {
        size_t name_length = wcslen(names[index]);
        size_t value_length = wcslen(values[index]);
        memcpy(cursor, names[index], name_length * sizeof(*cursor));
        cursor += name_length;
        *cursor++ = L'=';
        memcpy(cursor, values[index], value_length * sizeof(*cursor));
        cursor += value_length;
        *cursor++ = L'\0';
        ++index;
    }
    *cursor = L'\0';
    FreeEnvironmentStringsW(inherited);
    return environment;
}

static int run_process(int argc, wchar_t **argv, const wchar_t *adapter_profile,
    const wchar_t *adapter_root, const wchar_t *include_drives,
    const wchar_t *exclude_drives, const wchar_t *launch_plan,
    const wchar_t *config_source, const wchar_t *autoexec_source,
    const wchar_t *mutation_mode)
{
    STARTUPINFOW startup = { .cb = sizeof(startup) };
    PROCESS_INFORMATION process = {0};
    wchar_t *line = build_command_line(argc, argv);
    DWORD exit_code = 1u;
    HANDLE waits[2];
    DWORD wait_result;
    wchar_t *environment = NULL;
    SECURITY_ATTRIBUTES child_handle = { .nLength = sizeof(child_handle),
        .lpSecurityDescriptor = NULL, .bInheritHandle = TRUE };
    HANDLE eof_input = INVALID_HANDLE_VALUE;

    if (line == NULL) return 1;
    if (adapter_profile != NULL || adapter_root != NULL || include_drives != NULL || exclude_drives != NULL ||
        launch_plan != NULL || mutation_mode != NULL) {
        environment = build_adapter_environment(adapter_profile, adapter_root,
            include_drives, exclude_drives, launch_plan, config_source, autoexec_source,
            mutation_mode);
        if (environment == NULL) {
            HeapFree(GetProcessHeap(), 0, line);
            return 1;
        }
    }
    cancellation_event = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (cancellation_event == NULL || !SetConsoleCtrlHandler(handle_console_control, TRUE)) {
        if (cancellation_event != NULL) CloseHandle(cancellation_event);
        cancellation_event = NULL;
        HeapFree(GetProcessHeap(), 0, line);
        HeapFree(GetProcessHeap(), 0, environment);
        return 1;
    }
    eof_input = CreateFileW(L"NUL", GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, &child_handle, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (eof_input == INVALID_HANDLE_VALUE) {
        SetConsoleCtrlHandler(handle_console_control, FALSE);
        CloseHandle(cancellation_event);
        cancellation_event = NULL;
        HeapFree(GetProcessHeap(), 0, line);
        HeapFree(GetProcessHeap(), 0, environment);
        return 1;
    }
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = eof_input;
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    if (!CreateProcessW(argv[0], line, NULL, NULL, TRUE,
        CREATE_NEW_PROCESS_GROUP | (environment != NULL ? CREATE_UNICODE_ENVIRONMENT : 0u),
        environment, NULL,
        &startup, &process)) {
        fwprintf(stderr, L"ntdos64-run: CreateProcessW failed for %ls (%lu)\n",
            argv[0], GetLastError());
        CloseHandle(eof_input);
        SetConsoleCtrlHandler(handle_console_control, FALSE);
        CloseHandle(cancellation_event);
        cancellation_event = NULL;
        HeapFree(GetProcessHeap(), 0, line);
        HeapFree(GetProcessHeap(), 0, environment);
        return 1;
    }
    CloseHandle(eof_input);
    HeapFree(GetProcessHeap(), 0, line);
    HeapFree(GetProcessHeap(), 0, environment);
    waits[0] = process.hProcess;
    waits[1] = cancellation_event;
    wait_result = WaitForMultipleObjects(2u, waits, FALSE, INFINITE);
    if (wait_result == WAIT_OBJECT_0 + 1u) {
        /* The runner created this process group; cancellation has no broader
         * process, console-server, registry, or system configuration effect. */
        TerminateProcess(process.hProcess, STATUS_CONTROL_C_EXIT);
        WaitForSingleObject(process.hProcess, INFINITE);
    }
    GetExitCodeProcess(process.hProcess, &exit_code);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    SetConsoleCtrlHandler(handle_console_control, FALSE);
    CloseHandle(cancellation_event);
    cancellation_event = NULL;
    if (wait_result == WAIT_FAILED) return 1;
    return (int)exit_code;
}

/* Normalizes a non-empty comma-separated DOS-drive set to upper-case letters.
 * A later adapter consumer applies exclusion after inclusion; this parser only
 * admits stable, unambiguous CLI input and has no host-drive side effects. */
static int normalize_drive_list(const wchar_t *input, wchar_t output[52])
{
    unsigned char seen[26] = {0};
    size_t input_index = 0u, output_index = 0u;
    unsigned count = 0u;

    if (input == NULL || *input == L'\0') return 0;
    while (input[input_index] != L'\0') {
        wchar_t letter = input[input_index++];
        unsigned index;
        if ((letter < L'a' || letter > L'z') && (letter < L'A' || letter > L'Z')) return 0;
        if (letter >= L'a') letter = (wchar_t)(letter - L'a' + L'A');
        index = (unsigned)(letter - L'A');
        if (seen[index]) return 0;
        seen[index] = 1u;
        if (count != 0u) output[output_index++] = L',';
        output[output_index++] = letter;
        ++count;
        if (input[input_index] == L'\0') break;
        if (input[input_index++] != L',') return 0;
        if (input[input_index] == L'\0') return 0;
    }
    output[output_index] = L'\0';
    return count != 0u;
}

int wmain(void)
{
    int argc;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    const wchar_t *engine = NULL;
    const wchar_t *bochs = NULL;
    wchar_t product_root[MAX_PATH];
    wchar_t dos_root[MAX_PATH];
    wchar_t wow16_root[MAX_PATH];
    wchar_t config_source[MAX_PATH];
    wchar_t autoexec_source[MAX_PATH];
    const wchar_t *mutation_mode = L"direct";
    int has_mutation_mode = 0;
    wchar_t include_drives[52] = {0};
    wchar_t exclude_drives[52] = {0};
    int target_index = 1;
    image_kind kind;
    wchar_t full_path[MAX_PATH];
    wchar_t launch_plan[BYOB_LAUNCH_PLAN_V2_ENV_CHARS];
    ntdos64_startup_selection selection;
    byob_launch_plan_v2 launch;
    DWORD path_length;
    int result;

    if (argv == NULL || argc < 2) {
        fwprintf(stderr, L"usage: ntvdm64-0235 [--mutation-mode direct|readonly] [--include-drives c,d] [--exclude-drives e] [--engine engine.exe [--bochs ntdos64-bochs.exe]] target [args...]\n");
        if (argv != NULL) LocalFree(argv);
        return 2;
    }
    while (target_index < argc && wcsncmp(argv[target_index], L"--", 2u) == 0) {
        if (_wcsicmp(argv[target_index], L"--engine") == 0 &&
            target_index + 1 < argc && engine == NULL) {
            engine = argv[target_index + 1];
            target_index += 2;
        } else if (_wcsicmp(argv[target_index], L"--bochs") == 0 &&
            target_index + 1 < argc && bochs == NULL) {
            bochs = argv[target_index + 1];
            target_index += 2;
        } else if (_wcsicmp(argv[target_index], L"--include-drives") == 0 &&
            target_index + 1 < argc && include_drives[0] == L'\0' &&
            normalize_drive_list(argv[target_index + 1], include_drives)) {
            target_index += 2;
        } else if (_wcsicmp(argv[target_index], L"--exclude-drives") == 0 &&
            target_index + 1 < argc && exclude_drives[0] == L'\0' &&
            normalize_drive_list(argv[target_index + 1], exclude_drives)) {
            target_index += 2;
        } else if (_wcsicmp(argv[target_index], L"--mutation-mode") == 0 &&
            target_index + 1 < argc && !has_mutation_mode &&
            (_wcsicmp(argv[target_index + 1], L"direct") == 0 ||
             _wcsicmp(argv[target_index + 1], L"readonly") == 0)) {
            mutation_mode = _wcsicmp(argv[target_index + 1], L"readonly") == 0 ?
                L"readonly" : L"direct";
            has_mutation_mode = 1;
            target_index += 2;
        } else {
            fwprintf(stderr, L"ntdos64-run: invalid option\n");
            LocalFree(argv);
            return 2;
        }
    }
    if (target_index >= argc) {
        fwprintf(stderr, L"ntdos64-run: target is required\n");
        LocalFree(argv);
        return 2;
    }
    if (bochs != NULL && engine == NULL) {
        fwprintf(stderr, L"ntdos64-run: --bochs requires --engine\n");
        LocalFree(argv);
        return 2;
    }
    path_length = GetFullPathNameW(argv[target_index], MAX_PATH, full_path, NULL);
    if (path_length == 0u || path_length >= MAX_PATH) {
        fwprintf(stderr, L"ntdos64-run: cannot resolve target path\n");
        LocalFree(argv);
        return 2;
    }
    argv[target_index] = full_path;
    kind = classify_image(full_path);
    if (kind == IMAGE_KIND_PE32 || kind == IMAGE_KIND_PE64) {
        if (include_drives[0] != L'\0' || exclude_drives[0] != L'\0' || bochs != NULL ||
            has_mutation_mode) {
            fwprintf(stderr, L"ntdos64-run: DOS configuration options require an explicit DOS engine\n");
            result = 2;
        } else {
            result = run_process(argc - target_index, argv + target_index, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        }
    } else if ((kind == IMAGE_KIND_DOS || kind == IMAGE_KIND_COMMAND_INPUT) && engine != NULL) {
        int engine_argc = bochs == NULL ? 6 : 8;
        wchar_t **engine_argv = NULL;
        int index;

        memset(&selection, 0, sizeof(selection));
        selection.declared_target_count = 1u;
        selection.has_target_placement = 1u;
        selection.target_placement.drive_index = 2u;
        if (wcslen(wcsrchr(full_path, L'\\') != NULL ? wcsrchr(full_path, L'\\') + 1u : full_path) >=
                sizeof(selection.target.file_name) / sizeof(selection.target.file_name[0]) ||
            swprintf(selection.target.file_name,
                sizeof(selection.target.file_name) / sizeof(selection.target.file_name[0]),
                L"%ls", wcsrchr(full_path, L'\\') != NULL ? wcsrchr(full_path, L'\\') + 1u : full_path) < 0 ||
            (selection.declared_targets[0].component = selection.target,
             selection.declared_targets[0].placement = selection.target_placement, 0) ||
            !ntdos64_bundle_load_sibling(product_root, dos_root, wow16_root, config_source,
                autoexec_source) || !byob_launch_plan_v2_from_arguments(&launch, &selection,
                argc - target_index - 1, argv + target_index + 1) ||
            !byob_launch_plan_v2_to_environment(&launch, launch_plan)) {
            fwprintf(stderr, L"ntvdm64-0235: sibling dos/wow16 bundle validation failed\n");
            result = 3;
        }
        else {
            engine_argv = (wchar_t **)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                (size_t)engine_argc * sizeof(*engine_argv));
            if (engine_argv == NULL) {
                result = 1;
            } else {
                engine_argv[0] = (wchar_t *)engine;
                engine_argv[1] = L"--dos-root";
                engine_argv[2] = dos_root;
                engine_argv[3] = L"--wow16-root";
                engine_argv[4] = wow16_root;
                if (bochs != NULL) {
                    /* This remains runner-to-shim metadata. A native Bochs
                     * parser never receives a BYOB option or the bundle path. */
                    engine_argv[5] = L"--bochs";
                    engine_argv[6] = (wchar_t *)bochs;
                    engine_argv[7] = L"--";
                } else {
                    engine_argv[5] = L"--";
                }
                (void)index;
                result = run_process(engine_argc, engine_argv, full_path, dos_root,
                    include_drives, exclude_drives, launch_plan, config_source, autoexec_source,
                    mutation_mode);
                HeapFree(GetProcessHeap(), 0, engine_argv);
            }
        }
    } else if (kind == IMAGE_KIND_COMMAND_INPUT) {
        /* BAT/PIF are now valid COMMAND initial inputs, but the copied session
         * record is owned by the explicit DOS engine.  The CLI still neither
         * parses their bytes nor falls back to a host launch. */
        fwprintf(stderr, L"ntvdm64-0235: COMMAND initial input requires an explicit engine\n");
        result = 3;
    } else if (kind == IMAGE_KIND_NE) {
        fwprintf(stderr, L"ntdos64-run: NE targets require an unavailable Win16/WOW host path\n");
        result = 3;
    } else {
        fwprintf(stderr, L"ntdos64-run: target requires an explicit DOS engine\n");
        result = 3;
    }
    LocalFree(argv);
    return result;
}
