#ifndef RUN16_NATIVE_CONSOLE_BACKEND_H
#define RUN16_NATIVE_CONSOLE_BACKEND_H
#include "native_console_frame.h"
typedef struct ntkvm_terminal_frame ntkvm_terminal_frame;
typedef struct run16_native_backend run16_native_backend;
/* Root-frontend-owned object. Inner launchers submit to that root, never open
 * a backend themselves. Native target completion is a separate process handle. */
DWORD run16_native_backend_open(run16_native_backend **);
DWORD run16_native_backend_open_cancel(HANDLE stop,run16_native_backend **);
DWORD run16_native_backend_configure(run16_native_backend *,COORD,unsigned history);
DWORD run16_native_backend_seed(run16_native_backend *,const CONSOLE_SCREEN_BUFFER_INFOEX *,const CHAR_INFO *,unsigned,ULONGLONG *);
DWORD run16_native_backend_capture(run16_native_backend *,ntkvm_terminal_frame *);
DWORD run16_native_backend_screen_enter(run16_native_backend *);
void run16_native_backend_screen_leave(run16_native_backend *);
DWORD run16_native_backend_pump(run16_native_backend *);
DWORD run16_native_backend_control(run16_native_backend *,DWORD);
DWORD run16_native_backend_launch(run16_native_backend *,const run16_native_start *,HANDLE *);
DWORD run16_native_backend_input(run16_native_backend *,const INPUT_RECORD *,DWORD);
/* A record with any bytes delivered is consumed and must never be replayed.
 * ERROR_NO_MORE_ITEMS means clean EOF before delivery of the remaining input. */
DWORD run16_native_backend_input_some(run16_native_backend *,const INPUT_RECORD *,DWORD,DWORD *consumed);
/* Occupancy (zero or one), not a process count, descendants or completion code.
 * On failure leave the output untouched; never infer an empty session. */
DWORD run16_native_backend_members(run16_native_backend *,DWORD *);
void run16_native_backend_cancel(run16_native_backend *);
HANDLE run16_native_backend_process(run16_native_backend *); /* Stable borrowed EOF/error event. */
DWORD run16_native_backend_close(run16_native_backend *);
#endif
