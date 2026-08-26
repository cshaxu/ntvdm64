#include "opennt_command_oem_facade.h"

#include <string.h>

static CHAR test_system_directory[MAX_PATH + 1u];

UINT GetDriveTypeOem(LPSTR root)
{
    CHAR ansi[4];
    if (root == NULL || !OemToCharBuffA(root, ansi, 4u)) return DRIVE_UNKNOWN;
    return GetDriveTypeA(ansi);
}

DWORD GetEnvironmentVariableOem(LPSTR name, LPSTR buffer, DWORD bytes)
{
    CHAR ansi_name[4];
    CHAR ansi_value[MAX_PATH + 1u];
    DWORD result;
    if (name == NULL || buffer == NULL || bytes == 0u ||
        !OemToCharBuffA(name, ansi_name, (DWORD)(strlen(name) + 1u))) return 0u;
    result = GetEnvironmentVariableA(ansi_name, ansi_value, (DWORD)sizeof(ansi_value));
    if (result == 0u || result >= sizeof(ansi_value) ||
        !CharToOemBuffA(ansi_value, buffer, result + 1u)) return result;
    return result;
}

BOOL SetEnvironmentVariableOem(LPSTR name, LPSTR value)
{
    CHAR ansi_name[4];
    CHAR ansi_value[MAX_PATH + 1u];
    if (name == NULL || !OemToCharBuffA(name, ansi_name, (DWORD)(strlen(name) + 1u)))
        return FALSE;
    if (value == NULL) return SetEnvironmentVariableA(ansi_name, NULL);
    if (!OemToCharBuffA(value, ansi_value, (DWORD)(strlen(value) + 1u))) return FALSE;
    return SetEnvironmentVariableA(ansi_name, ansi_value);
}

UINT runtime_opennt_system_directory(LPSTR buffer, UINT bytes)
{
    if (test_system_directory[0] != '\0') {
        size_t length = strlen(test_system_directory);
        if (buffer == NULL || bytes == 0u) return (UINT)length;
        if (length >= bytes) return (UINT)length;
        memcpy(buffer, test_system_directory, length + 1u);
        return (UINT)length;
    }
    return GetSystemDirectoryA(buffer, bytes);
}

void runtime_opennt_set_system_directory(const CHAR *path)
{
    if (path == NULL) { test_system_directory[0] = '\0'; return; }
    strncpy(test_system_directory, path, MAX_PATH);
    test_system_directory[MAX_PATH] = '\0';
}

UINT runtime_command_misc_get_system_directory(LPSTR buffer, UINT bytes)
{
    return runtime_opennt_system_directory(buffer, bytes);
}

void runtime_command_misc_set_test_system_directory(const CHAR *path)
{
    runtime_opennt_set_system_directory(path);
}

/* DIVERGENCE(WIN32-DIV-010): NT4 exposed hidden `=X:` drive-directory
 * variables to VDM callers. Preserve the source fallback using the public
 * process current-directory API when the modern environment lacks that entry. */
DWORD runtime_opennt_command_environment_variable(LPSTR name, LPSTR buffer,
    DWORD bytes)
{
    DWORD result;
    if (name == NULL || buffer == NULL || bytes == 0u) return 0u;
    result = GetEnvironmentVariableA(name, buffer, bytes);
    if (result != 0u || name[0] != '=' || name[1] < 'A' || name[1] > 'Z' ||
        name[2] != ':' || name[3] != '\0') return result;
    result = GetCurrentDirectoryA(bytes, buffer);
    if (result == 0u || result >= bytes || buffer[1] != ':' ||
        (buffer[0] != name[1] && buffer[0] != (CHAR)(name[1] + ('a' - 'A'))))
        return 0u;
    return result;
}

/* DIVERGENCE(WIN32-DIV-011): the source private console export is absent
 * from public modern import libraries. GetKeyboardLayoutNameA gives the same
 * current-layout-name result without altering cmdkeyb.c's source algorithm. */
BOOL runtime_opennt_console_keyboard_layout_name(LPSTR name)
{
    return GetKeyboardLayoutNameA(name);
}
