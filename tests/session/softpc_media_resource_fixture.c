#include "ntvdm-exe/session/session.h"
#include "mvdm_softpc_firmware.h"

#include <windows.h>
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

    if (argc != 2) return 10;
    session_initialize(&instance, 1u);
    if (!session_set_mvdm_system_root(&instance, argv[1]) ||
        !session_activate(&instance) || !session_thread_bind(&instance)) return 1;
    if (!mvdm_softpc_system_copy_root(path, (uint32_t)sizeof(path)) ||
        strcmp(path, argv[1]) != 0) return 5;
    if (mvdm_softpc_system_copy_root(small_path, (uint32_t)sizeof(small_path)) ||
        small_path[0] != '\0') return 6;
    if (GetNtvdmWindowsDirectoryA(path, (uint32_t)sizeof(path)) !=
            strlen(argv[1]) || strcmp(path, argv[1]) != 0) return 7;
    if (GetNtvdmWindowsDirectoryA(small_path, (uint32_t)sizeof(small_path)) !=
            strlen(argv[1]) + 1u) return 8;
    if (!GetNtvdmSystemDirectoryA(system_path,
            (uint32_t)sizeof(system_path)) ||
        strstr(system_path, "system32") == NULL) return 9;
    if (!MultiByteToWideChar(CP_ACP, 0, argv[1], -1, expected_path,
            sizeof(expected_path) / sizeof(expected_path[0]))) return 10;
    if (!GetNtvdmWindowsDirectoryW(wide_path,
            (uint32_t)(sizeof(wide_path) / sizeof(wide_path[0]))) ||
        wcscmp(wide_path, expected_path) != 0) return 11;
    if (!GetNtvdmSystemDirectoryW(wide_path,
            (uint32_t)(sizeof(wide_path) / sizeof(wide_path[0]))) ||
        wcsstr(wide_path, L"system32") == NULL) return 12;
    if (!mvdm_softpc_system_find_file("NTIO.SYS", path,
            (uint32_t)sizeof(path)) || strstr(path, "NTIO.SYS") == NULL)
        return 2;
    if (mvdm_softpc_system_find_file("missing.sys", path,
            (uint32_t)sizeof(path)) || path[0] != '\0') return 3;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance))
        return 4;
    return 0;
}
