#ifndef OPENNT_CONSOLE_TITLE_H
#define OPENNT_CONSOLE_TITLE_H
#include <windows.h>
/* Original selected ANSI title callers; frontend owns the native Console. */
DWORD WINAPI MvdmGetConsoleTitleA(LPSTR,DWORD);
BOOL WINAPI MvdmSetConsoleTitleA(LPCSTR);
#define GetConsoleTitleA MvdmGetConsoleTitleA
#define SetConsoleTitleA MvdmSetConsoleTitleA
#endif
