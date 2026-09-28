#ifndef RUN16_NATIVE_CONSOLE_FRONTEND_H
#define RUN16_NATIVE_CONSOLE_FRONTEND_H
#include "native_console_backend.h"
#include "console_video.h"
typedef struct run16_native_frontend run16_native_frontend;
DWORD run16_native_frontend_create(run16_native_frontend **);
/* Caller owns non-inheritable duplicates of the session's canonical handles. */
DWORD run16_native_frontend_console(run16_native_frontend *,HANDLE *,HANDLE *);
/* Frontend-local presentation request; never changes target execution. */
DWORD run16_native_frontend_display(run16_native_frontend *,BOOL window);
DWORD run16_native_frontend_launch(run16_native_frontend *,const run16_native_start *,HANDLE *);
DWORD run16_native_frontend_wait(run16_native_frontend *,HANDLE,DWORD *);
void run16_native_frontend_destroy(run16_native_frontend *);
void run16_native_frontend_cancel(run16_native_frontend *);
DWORD run16_native_frontend_members(run16_native_frontend *,DWORD *);
DWORD run16_native_frontend_drain(run16_native_frontend *);
DWORD run16_native_frontend_dos_bind(run16_native_frontend *,const void *,BOOL);
/* Channel video is read only under the shared I/O lock. Forget/unbind must
 * detach it before channel storage is disposed. No pointer crosses IPC. */
DWORD run16_native_frontend_dos_video(run16_native_frontend *,const void *,const run16_console_video *);
/* Successful enter retains the shared I/O lock until leave. */
DWORD run16_native_frontend_dos_enter(run16_native_frontend *,const void *);
void run16_native_frontend_dos_leave(run16_native_frontend *);
DWORD run16_native_frontend_screen_begin(run16_native_frontend *);
DWORD run16_native_frontend_screen_end(run16_native_frontend *,BOOL);
/* Caller holds the successful dos_enter lock. */
BOOL run16_native_frontend_text_frame_required(run16_native_frontend *);
/* Borrowed readiness; data operations require the successful dos_enter lock. */
HANDLE run16_native_frontend_dos_ready(run16_native_frontend *);
DWORD run16_native_frontend_dos_read(run16_native_frontend *,BOOL,INPUT_RECORD *,DWORD,DWORD *);
DWORD run16_native_frontend_dos_prepend(run16_native_frontend *,const INPUT_RECORD *,DWORD);
void run16_native_frontend_dos_forget(run16_native_frontend *,const void *);
#endif
