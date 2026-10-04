#include "ntvdm-exe/session/session.h"
#include "mvdm_softpc_firmware.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

int main(int argc, char **argv)
{
    session instance;
    char path[SESSION_FIRMWARE_ROOT_BYTES + 32u];
    char system_path[SESSION_FIRMWARE_ROOT_BYTES + 32u];
    char small_path[4];
    wchar_t expected_path[SESSION_FIRMWARE_ROOT_BYTES + 32u];
    wchar_t wide_path[SESSION_FIRMWARE_ROOT_BYTES + 32u];

    if (argc != 2 && argc != 3) return 10;
    session_initialize(&instance, 1u);
    if (!session_set_mvdm_system_root(&instance, argv[1]) ||
        !session_activate(&instance) || !session_thread_bind(&instance)) return 1;
    if (argc == 3) {
        if (strcmp(argv[2], "--flat-copy-only") != 0 ||
            _snprintf_s(system_path, sizeof(system_path), _TRUNCATE,
                "%s\\NTIO.SYS", argv[1]) < 0 ||
            GetFileAttributesA(system_path) == INVALID_FILE_ATTRIBUTES)
            return 14;
        if (mvdm_softpc_system_find_file("NTIO.SYS", path, sizeof(path)) ||
            path[0] != '\0') return 15;
        return !session_thread_unbind(&instance) ||
            !session_dispose(&instance) ? 16 : 0;
    }
    if (!mvdm_softpc_system_copy_root(path, (uint32_t)sizeof(path)) ||
        strcmp(path, argv[1]) != 0) return 5;
    if (mvdm_softpc_system_copy_root(small_path, (uint32_t)sizeof(small_path)) ||
        small_path[0] != '\0') return 6;
    if (NtvdmGetWindowsDirectoryA(path, (uint32_t)sizeof(path)) !=
            strlen(argv[1]) || strcmp(path, argv[1]) != 0) return 7;
    if (NtvdmGetWindowsDirectoryA(small_path, (uint32_t)sizeof(small_path)) !=
            strlen(argv[1]) + 1u) return 8;
    if (!NtvdmGetSystemDirectoryA(system_path,
            (uint32_t)sizeof(system_path)) ||
        strstr(system_path, "system32") == NULL) return 9;
    if (!MultiByteToWideChar(CP_ACP, 0, argv[1], -1, expected_path,
            sizeof(expected_path) / sizeof(expected_path[0]))) return 10;
    if (!NtvdmGetWindowsDirectoryW(wide_path,
            (uint32_t)(sizeof(wide_path) / sizeof(wide_path[0]))) ||
        wcscmp(wide_path, expected_path) != 0) return 11;
    if (!NtvdmGetSystemDirectoryW(wide_path,
            (uint32_t)(sizeof(wide_path) / sizeof(wide_path[0]))) ||
        wcsstr(wide_path, L"system32") == NULL) return 12;
    if (!mvdm_softpc_system_find_file("NTIO.SYS", path,
            (uint32_t)sizeof(path)) ||
        _snprintf_s(system_path, sizeof(system_path), _TRUNCATE,
            "%s\\system32\\NTIO.SYS", argv[1]) < 0 ||
        _stricmp(path, system_path) != 0)
        return 2;
    if (!mvdm_softpc_system_find_file("system.ini", path,
            (uint32_t)sizeof(path)) ||
        _snprintf_s(system_path, sizeof(system_path), _TRUNCATE,
            "%s\\system.ini", argv[1]) < 0 ||
        _stricmp(path, system_path) != 0) return 13;
    if (mvdm_softpc_system_find_file("missing.sys", path,
            (uint32_t)sizeof(path)) || path[0] != '\0') return 3;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance))
        return 4;
    return 0;
}
