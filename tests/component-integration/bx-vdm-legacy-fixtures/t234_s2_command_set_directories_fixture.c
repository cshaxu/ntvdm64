#include <stdio.h>
#include <string.h>

#include "opennt-bop/command/opennt_command_composition.h"

int main(void)
{
    CHAR current[MAX_PATH + 1u];
    CHAR value[MAX_PATH + 1u];
    CHAR drive_environment[4] = "=?:";
    CHAR inherited_environment[] = "=C:=C:\\OPENNT\\INHERITED\0\0";
    VDMINFO info;
    DWORD current_bytes;
    DWORD value_bytes;
    CHAR drive;
    CHAR inherited_drive;

    current_bytes = GetCurrentDirectoryA((DWORD)sizeof(current), current);
    if (current_bytes == 0u || current_bytes >= sizeof(current) || current[1] != ':')
        return 1;
    drive = (CHAR)((current[0] >= 'a' && current[0] <= 'z') ?
        (current[0] - 'a' + 'A') : current[0]);
    if (drive < 'A' || drive > 'Z') return 2;
    inherited_drive = drive == 'C' ? 'D' : 'C';
    inherited_environment[1] = inherited_drive;
    inherited_environment[3] = inherited_drive;

    memset(&info, 0, sizeof(info));
    info.CurDrive = (USHORT)(drive - 'A');
    info.CurDirectory = current;
    info.CurDirectoryLen = (USHORT)current_bytes;
    cmdSetDirectories(inherited_environment, &info);

    drive_environment[1] = drive;
    value_bytes = GetEnvironmentVariableA(drive_environment, value, (DWORD)sizeof(value));
    if (value_bytes == 0u || value_bytes >= sizeof(value) || strcmp(value, current) != 0)
        return 3;
    drive_environment[1] = inherited_drive;
    value_bytes = GetEnvironmentVariableA(drive_environment, value, (DWORD)sizeof(value));
    if (value_bytes == 0u || value_bytes >= sizeof(value) ||
        strcmp(value, inherited_environment + 4u) != 0)
        return 4;
    if (GetCurrentDirectoryA((DWORD)sizeof(value), value) != current_bytes ||
        strcmp(value, current) != 0)
        return 5;

    puts("T234 S2 OpenNT cmdSetDirectories synchronization verified");
    return 0;
}
