#include <windows.h>
#include <wchar.h>

/* Test-only child. Its exit code proves the runner handed a validated profile
 * and root to an external engine before the target separator. */
int wmain(int argc, wchar_t **argv)
{
    DWORD profile_attributes;
    DWORD root_attributes;
    wchar_t profile[MAX_PATH];
    wchar_t root[MAX_PATH];
    wchar_t include_drives[64];
    wchar_t exclude_drives[64];
    wchar_t launch_plan[128];
    wchar_t config_source[MAX_PATH];
    wchar_t autoexec_source[MAX_PATH];
    char input_byte;
    DWORD input_bytes = 0u;

    if (argc != 6 || wcscmp(argv[1], L"--ntvdmcfg") != 0 ||
        wcscmp(argv[3], L"--config-root") != 0 || wcscmp(argv[5], L"--") != 0) {
        return 1;
    }
    profile_attributes = GetFileAttributesW(argv[2]);
    root_attributes = GetFileAttributesW(argv[4]);
    if (profile_attributes == INVALID_FILE_ATTRIBUTES ||
        (root_attributes & FILE_ATTRIBUTE_DIRECTORY) == 0u) {
        return 2;
    }
    if (GetEnvironmentVariableW(L"NTDOS64_ADAPTER_PROFILE", profile,
        sizeof(profile) / sizeof(profile[0])) == 0u ||
        GetEnvironmentVariableW(L"NTDOS64_ADAPTER_ROOT", root,
        sizeof(root) / sizeof(root[0])) == 0u ||
        GetEnvironmentVariableW(L"NTDOS64_HOST_INCLUDE_DRIVES", include_drives,
        sizeof(include_drives) / sizeof(include_drives[0])) == 0u ||
        GetEnvironmentVariableW(L"NTDOS64_HOST_EXCLUDE_DRIVES", exclude_drives,
        sizeof(exclude_drives) / sizeof(exclude_drives[0])) == 0u ||
        GetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_PLAN", launch_plan,
            sizeof(launch_plan) / sizeof(launch_plan[0])) == 0u ||
        GetEnvironmentVariableW(L"NTDOS64_STARTUP_CONFIG_SOURCE", config_source,
            sizeof(config_source) / sizeof(config_source[0])) == 0u ||
        GetEnvironmentVariableW(L"NTDOS64_STARTUP_AUTOEXEC_SOURCE", autoexec_source,
            sizeof(autoexec_source) / sizeof(autoexec_source[0])) == 0u ||
        wcscmp(profile, argv[2]) != 0 || wcscmp(root, argv[4]) != 0 ||
        wcscmp(include_drives, L"C,D,E") != 0 || wcscmp(exclude_drives, L"E") != 0 ||
        (wcscmp(launch_plan, L"2,1,c,082f6320736d6f6b65") != 0 &&
         wcscmp(launch_plan, L"2,1,p,00") != 0) ||
        config_source[0] == L'\0' || autoexec_source[0] == L'\0') {
        return 3;
    }
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), &input_byte, 1u,
            &input_bytes, NULL) || input_bytes != 0u) {
        return 4;
    }
    return 47;
}
