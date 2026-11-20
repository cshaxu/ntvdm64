#ifndef COMPAT_CONSOLE_SERVICES_H
#define COMPAT_CONSOLE_SERVICES_H

#include <windows.h>
#include <stdint.h>

UINT WINAPI MvdmGetConsoleCP(void);
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name);
DWORD WINAPI MvdmGetConsoleTitleA(LPSTR title, DWORD capacity);
BOOL WINAPI MvdmSetConsoleTitleA(LPCSTR title);

/* -1 no endpoint, 0 failure, 1 copied result. */
int ntvdm_console_window_query(DWORD query,LONG values[4]);
BOOL ntvdm_console_text_palette(PALETTEENTRY colours[16]);

BOOL ntvdm_console_prepend_keys(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);
BOOL ntvdm_console_prepend_keys_tagged(HANDLE,PINPUT_RECORD,const uint64_t *,DWORD,LPDWORD);

#endif
