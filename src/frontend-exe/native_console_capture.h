#ifndef RUN16_NATIVE_CONSOLE_CAPTURE_H
#define RUN16_NATIVE_CONSOLE_CAPTURE_H
#include <windows.h>

/* Backend-local lease. Only info/cursor/modes and bounded CHAR_INFO tiles cross
 * to the root presenter; the active-buffer HANDLE never crosses the wire. */
typedef struct run16_native_capture {
    HANDLE buffer;
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    CONSOLE_CURSOR_INFO cursor;
    DWORD output_mode,input_codepage,output_codepage;
} run16_native_capture;

DWORD run16_native_capture_begin(run16_native_capture *);
DWORD run16_native_capture_read(run16_native_capture *,DWORD offset,
    CHAR_INFO *,DWORD capacity,SMALL_RECT *,DWORD *count);
void run16_native_capture_end(run16_native_capture *);
/* The same public Console binding is used for initial hidden-buffer seeding
 * and visible presentation. Neither operation changes an execution lifetime. */
DWORD run16_native_screen_apply(HANDLE,const CONSOLE_SCREEN_BUFFER_INFOEX *,const CONSOLE_CURSOR_INFO *);
DWORD run16_native_cells_write(HANDLE,DWORD,const CHAR_INFO *,DWORD);
#endif
