#ifndef NTW32_CONSOLE_STATE_H
#define NTW32_CONSOLE_STATE_H
#include <windows.h>

/* Backend-local lease. Only info/cursor/modes and bounded CHAR_INFO tiles cross
 * to the root presenter; the active-buffer HANDLE never crosses the wire. */
typedef struct ntw32_capture {
    HANDLE buffer;
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    CONSOLE_CURSOR_INFO cursor;
    DWORD output_mode,input_codepage,output_codepage;
} ntw32_capture;

DWORD ntw32_capture_begin(ntw32_capture *);
/* One-time invisible carrier setup, before targets exist. Fixed font metrics
 * keep ordinary native resize APIs independent of inherited host font size. */
DWORD ntw32_console_initialize(void);
void ntw32_trace_error(const char *stage,DWORD operation,DWORD error);
/* Explicit process-local buffer; no HANDLE is serialized. */
DWORD ntw32_capture_begin_output(ntw32_capture *,HANDLE);
DWORD ntw32_capture_read(ntw32_capture *,DWORD offset,
    CHAR_INFO *,DWORD capacity,SMALL_RECT *,DWORD *count);
void ntw32_capture_end(ntw32_capture *);
/* Apply the acknowledged screen to this worker's invisible Console carrier.
 * Visible presentation and its bitmap font remain entirely NTKVM-owned. */
DWORD ntw32_screen_apply(HANDLE,const CONSOLE_SCREEN_BUFFER_INFOEX *,const CONSOLE_CURSOR_INFO *);
DWORD ntw32_cells_write(HANDLE,DWORD,const CHAR_INFO *,DWORD);
/* One completion-boundary resource check, not a task census. The caller holds
 * the signalled direct target HANDLE, pinning completed_target against reuse.
 * Failure or any other live attachment prevents automatic CloseOnExit. */
BOOL ntw32_console_quiescent(DWORD completed_target);
/* Explicit management shutdown only. Detach this carrier, request normal
 * Console close, and confirm the owned window disappeared before success. */
DWORD ntw32_console_close(void);
/* UI-side control processing recovered from S8; the Console still owns line
 * editing and signal delivery. written counts accepted records, not consumption. */
DWORD ntw32_input_write(HANDLE,const INPUT_RECORD *,DWORD count,DWORD *written);
#endif
