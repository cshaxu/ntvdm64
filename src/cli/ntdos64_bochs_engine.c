#include <windows.h>
#include <bcrypt.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#pragma comment(lib, "bcrypt.lib")

#define BUNDLE_MANIFEST L"ntdos64-bochs-engine-bundle-v1.txt"
#define BUNDLE_SCHEMA "ntdos64-bochs-engine-bundle-v1"
#define ROM_DIRECTORY L"ROM"
#define BIOS_NAME L"BIOS-bochs-latest"
#define VGA_NAME L"VGABIOS-lgpl-latest"
#define OPTION_ROM_NAME L"ntdos64-post-handoff.rom"

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
    int quoted = *argument == L'\0';
    const wchar_t *scan;
    for (scan = argument; *scan != L'\0'; ++scan)
        if (*scan == L' ' || *scan == L'\t' || *scan == L'"') quoted = 1;
    if (!quoted) return append_text(cursor, remaining, argument);
    if (*remaining < 2u) return 0;
    *(*cursor)++ = L'"'; --*remaining;
    while (*argument != L'\0') {
        size_t slashes = 0u;
        while (*argument == L'\\') { ++slashes; ++argument; }
        if (*argument == L'"') {
            while (slashes++ * 2u + 1u != 0u) {
                if (*remaining < 2u) return 0;
                *(*cursor)++ = L'\\'; --*remaining;
            }
            if (*remaining < 2u) return 0;
            *(*cursor)++ = L'"'; --*remaining; ++argument;
        } else {
            while (slashes-- != 0u) {
                if (*remaining < 2u) return 0;
                *(*cursor)++ = L'\\'; --*remaining;
            }
            if (*argument != L'\0') {
                if (*remaining < 2u) return 0;
                *(*cursor)++ = *argument++; --*remaining;
            }
        }
    }
    if (*remaining < 2u) return 0;
    *(*cursor)++ = L'"'; --*remaining;
    return 1;
}

static wchar_t *build_command_line(const wchar_t *program, const wchar_t *config)
{
    wchar_t *line = (wchar_t *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        (wcslen(program) * 2u + wcslen(config) * 2u + 16u) * sizeof(*line));
    wchar_t *cursor = line;
    size_t remaining = wcslen(program) * 2u + wcslen(config) * 2u + 16u;
    if (line == NULL || !append_argument(&cursor, &remaining, program) ||
        !append_text(&cursor, &remaining, L" -q -f ") ||
        !append_argument(&cursor, &remaining, config)) {
        HeapFree(GetProcessHeap(), 0, line);
        return NULL;
    }
    *cursor = L'\0';
    return line;
}

static int join_path(wchar_t *out, size_t count, const wchar_t *left, const wchar_t *right)
{
    size_t left_length = wcslen(left), right_length = wcslen(right);
    if (left_length + 1u + right_length + 1u > count) return 0;
    memcpy(out, left, left_length * sizeof(*out));
    out[left_length] = L'\\';
    memcpy(out + left_length + 1u, right, (right_length + 1u) * sizeof(*out));
    return 1;
}

static int sha256_file(const wchar_t *path, char result[65])
{
    HANDLE file = INVALID_HANDLE_VALUE;
    BCRYPT_ALG_HANDLE algorithm = NULL;
    BCRYPT_HASH_HANDLE hash = NULL;
    DWORD object_size = 0u, value_size = 0u, read = 0u;
    PUCHAR object = NULL;
    unsigned char digest[32];
    unsigned char buffer[4096];
    static const char hex[] = "0123456789ABCDEF";
    int ok = 0;
    DWORD index;

    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE ||
        BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, NULL, 0u) != 0 ||
        algorithm == NULL ||
        BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, (PUCHAR)&object_size,
            sizeof(object_size), &value_size, 0u) != 0 || object_size == 0u) goto done;
    object = (PUCHAR)HeapAlloc(GetProcessHeap(), 0u, object_size);
    if (object == NULL || BCryptCreateHash(algorithm, &hash, object, object_size,
        NULL, 0u, 0u) != 0) goto done;
    for (;;) {
        if (!ReadFile(file, buffer, sizeof(buffer), &read, NULL)) goto done;
        if (read == 0u) break;
        if (BCryptHashData(hash, buffer, read, 0u) != 0) goto done;
    }
    if (BCryptFinishHash(hash, digest, sizeof(digest), 0u) != 0) goto done;
    for (index = 0u; index < sizeof(digest); ++index) {
        result[index * 2u] = hex[digest[index] >> 4u];
        result[index * 2u + 1u] = hex[digest[index] & 15u];
    }
    result[64] = '\0';
    ok = 1;
done:
    if (hash != NULL) BCryptDestroyHash(hash);
    if (algorithm != NULL) BCryptCloseAlgorithmProvider(algorithm, 0u);
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    HeapFree(GetProcessHeap(), 0, object);
    return ok;
}

/* The manifest is intentionally tiny and fixed-order: it identifies exactly
 * the five immutable bundle inputs without creating a configuration language. */
static int read_manifest(const wchar_t *path, char values[5][65])
{
    static const char *const prefixes[] = {
        "schema=", "engine-sha256=", "bios-sha256=", "vga-sha256=", "optionrom-sha256="
    };
    HANDLE file;
    DWORD size, read = 0u;
    char buffer[512];
    char *line = buffer;
    unsigned index;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || (size = GetFileSize(file, NULL)) == INVALID_FILE_SIZE ||
        size == 0u || size >= sizeof(buffer) || !ReadFile(file, buffer, size, &read, NULL) || read != size) {
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        return 0;
    }
    CloseHandle(file);
    buffer[size] = '\0';
    for (index = 0u; index < 5u; ++index) {
        char *newline = strstr(line, "\n");
        char *end = newline;
        size_t prefix_length = strlen(prefixes[index]);
        if (end == NULL) end = line + strlen(line);
        if (end > line && end[-1] == '\r') --end;
        *end = '\0';
        if (strncmp(line, prefixes[index], prefix_length) != 0) return 0;
        if (index == 0u) {
            if (strcmp(line + prefix_length, BUNDLE_SCHEMA) != 0) return 0;
        } else {
            size_t length = strlen(line + prefix_length);
            if (length != 64u) return 0;
            memcpy(values[index - 1u], line + prefix_length, 65u);
        }
        line = newline == NULL ? end : newline + 1u;
    }
    return *line == '\0';
}

static int copy_verified(const wchar_t *source, const wchar_t *destination, const char expected[65])
{
    char actual[65];
    return sha256_file(source, actual) && strcmp(actual, expected) == 0 &&
        CopyFileW(source, destination, TRUE) && sha256_file(destination, actual) &&
        strcmp(actual, expected) == 0;
}

static int write_bochsrc(const wchar_t *path)
{
    static const char config[] =
        "config_interface: textconfig\r\n"
        "display_library: nogui\r\n"
        "cpu: count=1, ips=1000000, reset_on_triple_fault=0\r\n"
        "megs: 4\r\n"
        "romimage: file=ROM/BIOS-bochs-latest\r\n"
        "vgaromimage: file=ROM/VGABIOS-lgpl-latest\r\n"
        "optromimage1: file=ROM/ntdos64-post-handoff.rom, address=0xca800\r\n"
        "pci: enabled=0\r\n"
        "vga: extension=none, update_freq=5\r\n"
        "floppy_bootsig_check: disabled=1\r\n"
        "boot: disk\r\n"
        "plugin_ctrl: unmapped=0, biosdev=0, speaker=0, extfpuirq=0, parallel=0, serial=0, gameport=0\r\n";
    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0u, NULL, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD written = 0u;
    if (file == INVALID_HANDLE_VALUE || !WriteFile(file, config, (DWORD)(sizeof(config) - 1u), &written, NULL) ||
        written != sizeof(config) - 1u) {
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        return 0;
    }
    CloseHandle(file);
    return 1;
}

static int run_bochs(const wchar_t *bochs, const wchar_t *run_directory)
{
    STARTUPINFOW startup = { .cb = sizeof(startup), .dwFlags = STARTF_USESTDHANDLES };
    PROCESS_INFORMATION process = {0};
    wchar_t *line = build_command_line(bochs, L"bochsrc");
    DWORD exit_code = 1u;
    HANDLE eof_input = INVALID_HANDLE_VALUE;
    /* Native Bochs's outer Win32 wrapper may read stdin after the CPU loop.
     * The noninteractive CLI owns this child-only EOF policy; it is not an
     * adapter/Bochs ABI or a guest-visible capability. */
    eof_input = CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, 0u, NULL);
    if (eof_input == INVALID_HANDLE_VALUE ||
        !SetHandleInformation(eof_input, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT)) {
        if (eof_input != INVALID_HANDLE_VALUE) CloseHandle(eof_input);
        HeapFree(GetProcessHeap(), 0, line);
        return 1;
    }
    startup.hStdInput = eof_input;
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    /* This is a fixed engine-profile opt-in, not caller-provided Bochs
     * configuration. The generic Bochs #UD path remains selector-blind; the
     * selected adapter runtime owns all typed result dispatch. */
    if (!SetEnvironmentVariableW(L"NTDOS64_ADAPTER_BRIDGE", L"1")) {
        HeapFree(GetProcessHeap(), 0, line);
        return 1;
    }
    if (line == NULL || !CreateProcessW(bochs, line, NULL, NULL, TRUE,
        CREATE_NEW_PROCESS_GROUP, NULL, run_directory, &startup, &process)) {
        fwprintf(stderr, L"ntdos64-bochs-engine: native Bochs launch failed (%lu)\n", GetLastError());
        CloseHandle(eof_input);
        HeapFree(GetProcessHeap(), 0, line);
        return 1;
    }
    CloseHandle(eof_input);
    HeapFree(GetProcessHeap(), 0, line);
    WaitForSingleObject(process.hProcess, INFINITE);
    GetExitCodeProcess(process.hProcess, &exit_code);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return (int)exit_code;
}

int wmain(int argc, wchar_t **argv)
{
    wchar_t bochs_full[MAX_PATH], bundle[MAX_PATH], manifest[MAX_PATH], rom_source[MAX_PATH];
    wchar_t run_directory[MAX_PATH], run_rom[MAX_PATH], destination[MAX_PATH], config[MAX_PATH];
    char hashes[5][65];
    const wchar_t *names[] = { BIOS_NAME, VGA_NAME, OPTION_ROM_NAME };
    const wchar_t *failure = L"argument";
    DWORD length;
    unsigned index;

    if (argc != 8 || wcscmp(argv[1], L"--byob-profile") != 0 ||
        wcscmp(argv[3], L"--byob-root") != 0 || wcscmp(argv[5], L"--bochs") != 0 ||
        wcscmp(argv[7], L"--") != 0 || *argv[2] == L'\0' || *argv[4] == L'\0') {
        fwprintf(stderr, L"usage: ntdos64-bochs-engine --byob-profile profile --byob-root root --bochs bundle\\ntdos64-bochs.exe --\n");
        return 2;
    }
    length = GetFullPathNameW(argv[6], MAX_PATH, bochs_full, NULL);
    if (length == 0u || length >= MAX_PATH || GetFileAttributesW(bochs_full) == INVALID_FILE_ATTRIBUTES ||
        _wcsicmp(wcsrchr(bochs_full, L'\\') + 1, L"ntdos64-bochs.exe") != 0) goto reject;
    failure = L"bundle manifest";
    wcscpy(bundle, bochs_full);
    *wcsrchr(bundle, L'\\') = L'\0';
    if (!join_path(manifest, MAX_PATH, bundle, BUNDLE_MANIFEST) || !read_manifest(manifest, hashes)) goto reject;
    failure = L"engine SHA-256";
    if (!sha256_file(bochs_full, hashes[4]) || strcmp(hashes[4], hashes[0]) != 0) goto reject;
    failure = L"ROM directory";
    if (!join_path(rom_source, MAX_PATH, bundle, ROM_DIRECTORY)) goto reject;
    failure = L"private run directory";
    if (!GetTempPathW(MAX_PATH, run_directory) || !GetTempFileNameW(run_directory, L"n64", 0u, run_directory) ||
        !DeleteFileW(run_directory) || !CreateDirectoryW(run_directory, NULL) ||
        !join_path(run_rom, MAX_PATH, run_directory, ROM_DIRECTORY) || !CreateDirectoryW(run_rom, NULL)) goto reject;
    for (index = 0u; index < 3u; ++index) {
        wchar_t source[MAX_PATH];
        failure = names[index];
        if (!join_path(source, MAX_PATH, rom_source, names[index]) ||
            !join_path(destination, MAX_PATH, run_rom, names[index]) ||
            !copy_verified(source, destination, hashes[index + 1u])) goto reject;
    }
    failure = L"bochsrc";
    if (!join_path(config, MAX_PATH, run_directory, L"bochsrc") || !write_bochsrc(config)) goto reject;
    return run_bochs(bochs_full, run_directory);
reject:
    fwprintf(stderr, L"ntdos64-bochs-engine: fixed engine bundle validation failed (%ls)\n", failure);
    return 3;
}
