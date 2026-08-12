#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

static int read_exact(const wchar_t *path, const char *expected)
{
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    char buffer[512];
    DWORD read = 0u;
    size_t length = strlen(expected);
    int ok = file != INVALID_HANDLE_VALUE && length <= sizeof(buffer) &&
        ReadFile(file, buffer, (DWORD)length, &read, NULL) &&
        read == length && memcmp(buffer, expected, length) == 0;
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    return ok;
}

int wmain(int argc, wchar_t **argv)
{
    wchar_t config[MAX_PATH], root[MAX_PATH], path[MAX_PATH];
    DWORD length;
    char input;
    DWORD read = 0u;
    static const char expected_config[] =
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
    if (argc != 4 || wcscmp(argv[1], L"-q") != 0 || wcscmp(argv[2], L"-f") != 0 ||
        wcscmp(argv[3], L"bochsrc") != 0) return 1;
    length = GetFullPathNameW(argv[3], MAX_PATH, config, NULL);
    if (length == 0u || length >= MAX_PATH || !read_exact(config, expected_config)) return 2;
    wcscpy(root, config);
    *wcsrchr(root, L'\\') = L'\0';
    if (swprintf(path, MAX_PATH, L"%ls\\ROM\\BIOS-bochs-latest", root) < 0 ||
        !read_exact(path, "fake-bios\r\n")) return 3;
    if (swprintf(path, MAX_PATH, L"%ls\\ROM\\VGABIOS-lgpl-latest", root) < 0 ||
        !read_exact(path, "fake-vga\r\n")) return 4;
    if (swprintf(path, MAX_PATH, L"%ls\\ROM\\ntdos64-post-handoff.rom", root) < 0 ||
        !read_exact(path, "fake-option-rom\r\n")) return 5;
    if (GetEnvironmentVariableW(L"NTDOS64_ADAPTER_PROFILE", path,
        sizeof(path) / sizeof(path[0])) == 0u ||
        GetEnvironmentVariableW(L"NTDOS64_ADAPTER_ROOT", path,
        sizeof(path) / sizeof(path[0])) == 0u ||
        GetEnvironmentVariableW(L"NTDOS64_ADAPTER_BRIDGE", path,
        sizeof(path) / sizeof(path[0])) == 0u || wcscmp(path, L"1") != 0 ||
        !ReadFile(GetStdHandle(STD_INPUT_HANDLE), &input, 1u, &read, NULL) || read != 0u) return 6;
    return 47;
}
