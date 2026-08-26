#ifndef ADAPTER_WIN32_OPENNT_COMMAND_OEM_FACADE_H
#define ADAPTER_WIN32_OPENNT_COMMAND_OEM_FACADE_H

#include <windows.h>

UINT GetDriveTypeOem(LPSTR root);
DWORD GetEnvironmentVariableOem(LPSTR name, LPSTR buffer, DWORD bytes);
BOOL SetEnvironmentVariableOem(LPSTR name, LPSTR value);
UINT runtime_command_misc_get_system_directory(LPSTR buffer, UINT bytes);
void runtime_command_misc_set_test_system_directory(const CHAR *path);
DWORD runtime_opennt_command_environment_variable(LPSTR name, LPSTR buffer,
    DWORD bytes);
BOOL runtime_opennt_console_keyboard_layout_name(LPSTR name);

#endif
