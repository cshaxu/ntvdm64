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

UINT runtime_command_misc_get_system_directory(LPSTR buffer, UINT bytes)
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

void runtime_command_misc_set_test_system_directory(const CHAR *path)
{
    if (path == NULL) { test_system_directory[0] = '\0'; return; }
    strncpy(test_system_directory, path, MAX_PATH);
    test_system_directory[MAX_PATH] = '\0';
}
