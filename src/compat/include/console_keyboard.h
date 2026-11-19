#ifndef COMPAT_CONSOLE_KEYBOARD_H
#define COMPAT_CONSOLE_KEYBOARD_H

#include <windows.h>

/* Product-owned Console routing, installed before original keyboard queries.
 * The immutable table owns neither a session nor a Console channel. */
typedef struct compat_console_keyboard {
    UINT (WINAPI *code_page)(void);
    BOOL (WINAPI *layout_name)(LPSTR);
} compat_console_keyboard;

BOOL compat_console_keyboard_install(const compat_console_keyboard *services);
UINT WINAPI MvdmGetConsoleCP(void);
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name);

#endif
