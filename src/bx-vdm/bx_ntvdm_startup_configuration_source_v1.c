#include "bx_ntvdm_startup_configuration_source_v1.h"

#include <string.h>
#include <windows.h>

#define BX_NTVDM_CONFIG_ENV L"NTDOS64_STARTUP_CONFIG_SOURCE"
#define BX_NTVDM_AUTOEXEC_ENV L"NTDOS64_STARTUP_AUTOEXEC_SOURCE"

static uint32_t shell_capability(const wchar_t *root)
{
    static const wchar_t suffix[] = L"\\System32\\command.com";
    wchar_t candidate[MAX_PATH];
    size_t root_chars;
    DWORD attributes;
    if (root == 0 || (root_chars = wcslen(root)) + wcslen(suffix) >= MAX_PATH)
        return BX_NTVDM_STARTUP_CONFIGURATION_SHELL_V1_DECLARED_GUEST;
    memcpy(candidate, root, root_chars * sizeof(*candidate));
    wcscpy_s(candidate + root_chars, MAX_PATH - root_chars, suffix);
    attributes = GetFileAttributesW(candidate);
    return attributes != INVALID_FILE_ATTRIBUTES &&
        (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0u ?
        BX_NTVDM_STARTUP_CONFIGURATION_SHELL_V1_ORIGINAL_HOST :
        BX_NTVDM_STARTUP_CONFIGURATION_SHELL_V1_DECLARED_GUEST;
}

static int read_oem(const wchar_t *path, uint8_t *out, uint32_t *bytes,
    int require_nonempty)
{
    HANDLE file; LARGE_INTEGER size; DWORD read = 0u;
    if (!path || !*path || !out || !bytes) return 0;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE || !GetFileSizeEx(file, &size) ||
        size.QuadPart < 0 || size.QuadPart > 4095) { if(file != INVALID_HANDLE_VALUE) CloseHandle(file); return 0; }
    if (!ReadFile(file, out, (DWORD)size.QuadPart, &read, 0) || read != (DWORD)size.QuadPart) { CloseHandle(file); return 0; }
    CloseHandle(file);
    *bytes = read;
    return !require_nonempty || *bytes != 0u;
}

int bx_ntvdm_startup_configuration_source_v1_from_environment(
    bx_ntvdm_startup_configuration_input_v1 *input,
    const bx_ntvdm_mutation_profile_v1 *profile)
{
    wchar_t config[MAX_PATH], autoexec[MAX_PATH], root[MAX_PATH];
    DWORD c, a, r; UINT cp;
    if (!input || !profile) return 0;
    c = GetEnvironmentVariableW(BX_NTVDM_CONFIG_ENV, config, MAX_PATH);
    a = GetEnvironmentVariableW(BX_NTVDM_AUTOEXEC_ENV, autoexec, MAX_PATH);
    if (!c || !a || c >= MAX_PATH || a >= MAX_PATH ||
        !(r = GetWindowsDirectoryW(root, MAX_PATH)) || r >= MAX_PATH ||
        !(cp = GetOEMCP())) return 0;
    memset(input, 0, sizeof(*input));
    input->magic = BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_MAGIC;
    input->abi_version = BX_NTVDM_STARTUP_CONFIGURATION_PROVIDER_V1_VERSION;
    input->struct_bytes = sizeof(*input);
    if (!bx_ntvdm_startup_configuration_policy_v1_initialize(&input->policy, profile,
            BX_NTVDM_STARTUP_CONFIGURATION_SOURCE_V1_EXPLICIT_HOST) ||
        !WideCharToMultiByte(CP_OEMCP, 0, root, -1, (char *)input->system_root,
            sizeof(input->system_root), 0, 0) ||
        !read_oem(config, input->config, &input->config_bytes, 1) ||
        !read_oem(autoexec, input->autoexec, &input->autoexec_bytes, 0)) return 0;
    input->system_root_bytes = (uint32_t)strlen((const char *)input->system_root);
    input->country_id = 1u; input->oem_code_page = cp;
    input->shell_capability = shell_capability(root);
    return bx_ntvdm_startup_configuration_input_v1_valid(input);
}
