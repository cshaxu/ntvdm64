#include "app/package_layout.h"

#include <string.h>
#include <windows.h>

int app_package_layout_set_firmware_root(session *owner,
    const char *executable_path)
{
    const char *separator;
    size_t directory_length;
    char root[SESSION_FIRMWARE_ROOT_BYTES];
    static const char suffix[] = "\\softpc\\roms";

    if (owner == NULL || executable_path == NULL ||
        executable_path[0] == '\0') return 0;
    separator = strrchr(executable_path, '\\');
    if (separator == NULL) separator = strrchr(executable_path, '/');
    if (separator == NULL) return 0;
    directory_length = (size_t)(separator - executable_path);
    if (directory_length == 0u || directory_length + sizeof(suffix) >
        sizeof(root)) return 0;
    memcpy(root, executable_path, directory_length);
    memcpy(root + directory_length, suffix, sizeof(suffix));
    return session_set_firmware_root(owner, root);
}

int app_package_layout_set_process_firmware_root(session *owner)
{
    char executable_path[SESSION_FIRMWARE_ROOT_BYTES];
    DWORD length = GetModuleFileNameA(NULL, executable_path,
        (DWORD)sizeof(executable_path));
    if (length == 0u || length >= sizeof(executable_path)) return 0;
    executable_path[length] = '\0';
    return app_package_layout_set_firmware_root(owner, executable_path);
}
