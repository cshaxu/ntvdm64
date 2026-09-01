#include "app/package_layout.h"

#include <string.h>
#include <windows.h>

#define APP_COMMAND_SHORT_ROOT_BYTES 64u
#define APP_COMMAND_SYSTEM_FILE_BYTES (MAX_PATH + 12u)

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
    if (!app_package_layout_make_root(executable_path, "\\mvdm\\softpc", root,
            sizeof(root))) return 0;
    return session_set_firmware_root(owner, root);
}

int app_package_layout_set_media_roots(session *owner,
    const char *executable_path)
{
    char system_root[SESSION_FIRMWARE_ROOT_BYTES];
    char firmware_root[SESSION_FIRMWARE_ROOT_BYTES];

    if (!app_package_layout_make_root(executable_path, "\\mvdm", system_root,
            sizeof(system_root)) ||
        !app_package_layout_make_root(executable_path, "\\mvdm\\softpc",
            firmware_root, sizeof(firmware_root))) return 0;
    return session_set_mvdm_system_root(owner, system_root) &&
        session_set_firmware_root(owner, firmware_root);
}

static int app_package_layout_command_file_fits(const char *root,
    const char *file_name)
{
    size_t root_bytes;
    size_t file_bytes;
    size_t separator_bytes;

    if (root == NULL || root[0] == '\0' || file_name == NULL ||
        file_name[0] == '\0') return 0;
    root_bytes = strlen(root);
    file_bytes = strlen(file_name);
    separator_bytes = root[root_bytes - 1u] != '\\' &&
        root[root_bytes - 1u] != '/' ? 1u : 0u;
    return root_bytes + separator_bytes + file_bytes + 1u <=
        APP_COMMAND_SYSTEM_FILE_BYTES;
}

int app_package_layout_validate_command_configuration_root(
    const session *owner)
{
    char short_root[APP_COMMAND_SHORT_ROOT_BYTES];
    DWORD short_root_bytes;
    const char *root = session_mvdm_system_root(owner);

    if (root == NULL || root[0] == '\0') return 0;
    /* DIVERGENCE(APP-DIV-014): keep imported cmdconf.c's 64-byte
     * GetShortPathNameA contract untouched.  App rejects a package before
     * original host startup only when the selected executable-relative root
     * cannot be represented by that existing contract; it does not create,
     * shorten, or substitute a path. */
    short_root_bytes = GetShortPathNameA(root, short_root,
        (DWORD)sizeof(short_root));
    if (short_root_bytes == 0u || short_root_bytes >= sizeof(short_root))
        return 0;
    return app_package_layout_command_file_fits(root, "config.nt") &&
        app_package_layout_command_file_fits(root, "autoexec.nt") &&
        app_package_layout_command_file_fits(root, "ntio.sys");
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
