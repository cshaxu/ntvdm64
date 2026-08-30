#include <stdio.h>
#include <windows.h>

#include "conapi.h"

HANDLE GetConsoleInputWaitHandle(VOID);

int main(void)
{
    DWORD count = 0u;
    BOOL state = FALSE;

    if (ReadConsoleInputExW(INVALID_HANDLE_VALUE, NULL, 0u, &count, 0x8000u) ||
        GetLastError() != ERROR_INVALID_PARAMETER) return 1;
    if (VDMConsoleOperation(0xffffffffu, &state) ||
        GetLastError() != ERROR_CALL_NOT_IMPLEMENTED) return 2;
    if (ShowConsoleCursor(INVALID_HANDLE_VALUE, TRUE) != -1) return 3;
    if (GetConsoleInputWaitHandle() == NULL) return 4;

    puts("PASS: console input compatibility failure contracts");
    return 0;
}
