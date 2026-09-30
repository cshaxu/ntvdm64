#ifndef NTCON_CONSOLE_STATE_H
#define NTCON_CONSOLE_STATE_H
#include <windows.h>

/* Backend-local lease. Only info/cursor/modes and bounded CHAR_INFO tiles cross
 * to the root presenter; the active-buffer HANDLE never crosses the wire. */
typedef struct ntcon_capture {
    HANDLE buffer;
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    CONSOLE_CURSOR_INFO cursor;
    DWORD output_mode,input_codepage,output_codepage;
} ntcon_capture;

DWORD ntcon_capture_begin(ntcon_capture *);
/* One-time invisible carrier setup, before targets exist. Fixed font metrics
 * keep ordinary native resize APIs independent of inherited host font size. */
DWORD ntcon_console_initialize(void);
void ntcon_trace_error(const char *stage,DWORD operation,DWORD error);
/* Explicit process-local buffer; no HANDLE is serialized. */
DWORD ntcon_capture_begin_output(ntcon_capture *,HANDLE);
DWORD ntcon_capture_read(ntcon_capture *,DWORD offset,
    CHAR_INFO *,DWORD capacity,SMALL_RECT *,DWORD *count);
void ntcon_capture_end(ntcon_capture *);
/* Apply the acknowledged screen to this worker's invisible Console carrier.
 * Visible presentation and its bitmap font remain entirely NTKVM-owned. */
DWORD ntcon_screen_apply(HANDLE,const CONSOLE_SCREEN_BUFFER_INFOEX *,const CONSOLE_CURSOR_INFO *);
DWORD ntcon_cells_write(HANDLE,DWORD,const CHAR_INFO *,DWORD);
/* Returns attached members excluding this backend; caller frees *members with
 * HeapFree(GetProcessHeap(),0,...). Failure never reports an empty session. */
DWORD ntcon_console_members(DWORD **members,DWORD *count);
/* Snapshot a currently attached native target image for management only.
 * It never chooses execution ownership or controls the target process. */
DWORD ntcon_console_active_image(WCHAR image[260]);
/* Explicit management shutdown only. Detach this carrier, request normal
 * Console close, and confirm the owned window disappeared before success. */
DWORD ntcon_console_close(void);
/* UI-side control processing recovered from S8; the Console still owns line
 * editing and signal delivery. written counts accepted records, not consumption. */
DWORD ntcon_input_write(HANDLE,const INPUT_RECORD *,DWORD count,DWORD *written);
#endif
