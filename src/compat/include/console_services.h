#ifndef COMPAT_CONSOLE_SERVICES_H
#define COMPAT_CONSOLE_SERVICES_H

#include <windows.h>

/* Product-owned Console routing, installed before original Console calls.
 * The immutable table owns neither a session nor a Console channel. */
typedef struct compat_console_services {
    UINT (WINAPI *code_page)(void);
    BOOL (WINAPI *layout_name)(LPSTR);
    DWORD (WINAPI *get_title)(LPSTR, DWORD);
    BOOL (WINAPI *set_title)(LPCSTR);
} compat_console_services;

BOOL compat_console_services_install(const compat_console_services *services);
UINT WINAPI MvdmGetConsoleCP(void);
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name);
DWORD WINAPI MvdmGetConsoleTitleA(LPSTR title, DWORD capacity);
BOOL WINAPI MvdmSetConsoleTitleA(LPCSTR title);

#endif
