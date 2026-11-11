#include <stdio.h>
#include <string.h>

#include <windows.h>
#include <oemuni.h>

int main(void)
{
    char directory[MAX_PATH + 1];
    char system_root[MAX_PATH + 1];
    DWORD directory_bytes;
    DWORD environment_bytes;

    memset(directory, 0, sizeof(directory));
    memset(system_root, 0, sizeof(system_root));
    directory_bytes = GetCurrentDirectoryOem((DWORD)sizeof(directory),
        directory);
    environment_bytes = GetEnvironmentVariableOem("SystemRoot", system_root,
        (DWORD)sizeof(system_root));
    if (directory_bytes == 0u || directory_bytes >= sizeof(directory) ||
        directory[directory_bytes] != '\0' || environment_bytes == 0u ||
        environment_bytes >= sizeof(system_root) ||
        system_root[environment_bytes] != '\0') return 1;
    puts("PASS: original OEM environment/current-directory helpers");
    return 0;
}
