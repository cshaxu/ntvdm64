#ifndef OPENNT_SUPPORT_WINCONP_H
#define OPENNT_SUPPORT_WINCONP_H

#include <windows.h>

BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR);
UINT WINAPI MvdmGetConsoleCP(void);
#define GetConsoleKeyboardLayoutName GetConsoleKeyboardLayoutNameA
#define GetConsoleCP MvdmGetConsoleCP

#endif
