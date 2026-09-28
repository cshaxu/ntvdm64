#ifndef RUN16_NATIVE_CONSOLE_CAPTURE_H
#define RUN16_NATIVE_CONSOLE_CAPTURE_H
#include <windows.h>

/* Visible Console presentation; no hidden-Console snapshot reader. */
DWORD run16_native_screen_apply(HANDLE,const CONSOLE_SCREEN_BUFFER_INFOEX *,const CONSOLE_CURSOR_INFO *);
DWORD run16_native_cells_write(HANDLE,DWORD,const CHAR_INFO *,DWORD);
#endif
