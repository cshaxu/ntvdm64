#include "app/package_layout.h"

#include <string.h>
#include <windows.h>

static int app_package_layout_make_root(const char *executable_path,
    const char *suffix, char *root, size_t root_bytes)
{
    const char *separator;
    size_t directory_length;

    if (executable_path == NULL || executable_path[0] == '\0' ||
        suffix == NULL || root == NULL || root_bytes == 0u) return 0;
    separator = strrchr(executable_path, '\\');
    if (separator == NULL) separator = strrchr(executable_path, '/');
    if (separator == NULL) return 0;
    directory_length = (size_t)(separator - executable_path);
    if (directory_length == 0u || directory_length + strlen(suffix) + 1u >
        root_bytes) return 0;
    memcpy(root, executable_path, directory_length);
    memcpy(root + directory_length, suffix, strlen(suffix) + 1u);
    return 1;
}

int app_package_layout_set_firmware_root(session *owner,
    const char *executable_path)
{
    char root[SESSION_FIRMWARE_ROOT_BYTES];
    if (!app_package_layout_make_root(executable_path, "\\softpc\\roms", root,
            sizeof(root))) return 0;
    return session_set_firmware_root(owner, root);
}

int app_package_layout_set_media_roots(session *owner,
    const char *executable_path)
{
    char dos_root[SESSION_FIRMWARE_ROOT_BYTES];
    char win16_root[SESSION_FIRMWARE_ROOT_BYTES];
    char firmware_root[SESSION_FIRMWARE_ROOT_BYTES];

    if (!app_package_layout_make_root(executable_path, "\\dos", dos_root,
            sizeof(dos_root)) ||
        !app_package_layout_make_root(executable_path, "\\win16", win16_root,
            sizeof(win16_root)) ||
        !app_package_layout_make_root(executable_path, "\\softpc\\roms",
            firmware_root, sizeof(firmware_root))) return 0;
    return session_set_dos_media_root(owner, dos_root) &&
        session_set_win16_media_root(owner, win16_root) &&
        session_set_firmware_root(owner, firmware_root);
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

int app_package_layout_set_process_media_roots(session *owner)
{
    char executable_path[SESSION_FIRMWARE_ROOT_BYTES];
    DWORD length = GetModuleFileNameA(NULL, executable_path,
        (DWORD)sizeof(executable_path));
    if (length == 0u || length >= sizeof(executable_path)) return 0;
    executable_path[length] = '\0';
    return app_package_layout_set_media_roots(owner, executable_path);
}
