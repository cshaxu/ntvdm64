#include <stdio.h>

#include "cmd.h"

extern BOOL CMDInit(INT argc, PSZ *argv);

int main(void)
{
    CHAR system_directory[MAX_PATH];
    UINT length;
    CHAR initial_drive = cmdHomeDirectory[0];

    if (!IsFirstCall || IsRepeatCall || cbComSpec != 0u ||
        cmdHomeDirectory[1] != ':') return 1;
    length = GetSystemDirectory(system_directory, MAX_PATH);
    if (!CMDInit(0, NULL)) return 2;
    if (length > 0u && length <= MAX_PATH &&
        cmdHomeDirectory[0] != system_directory[0]) return 3;
    if (length == 0u || length > MAX_PATH) {
        if (cmdHomeDirectory[0] != initial_drive) return 4;
    }
    puts("PASS: original COMMAND initialization composition");
    return 0;
}
