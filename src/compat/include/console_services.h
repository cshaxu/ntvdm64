#ifndef COMPAT_CONSOLE_SERVICES_H
#define COMPAT_CONSOLE_SERVICES_H

#include <windows.h>

UINT WINAPI MvdmGetConsoleCP(void);
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name);
DWORD WINAPI MvdmGetConsoleTitleA(LPSTR title, DWORD capacity);
BOOL WINAPI MvdmSetConsoleTitleA(LPCSTR title);

#endif
