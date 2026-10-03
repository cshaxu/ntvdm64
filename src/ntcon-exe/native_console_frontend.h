#ifndef RUN16_NATIVE_CONSOLE_FRONTEND_H
#define RUN16_NATIVE_CONSOLE_FRONTEND_H
#include <windows.h>
#include "console_video.h"
#include "common/protocol/console_io.h"
typedef struct run16_native_frontend run16_native_frontend;
DWORD run16_native_frontend_create(run16_native_frontend **);
/* Caller owns non-inheritable duplicates of the session's canonical handles. */
DWORD run16_native_frontend_console(run16_native_frontend *,HANDLE *,HANDLE *);
/* Both worker kinds use the frontend's private logical Console cell grid. */
DWORD run16_native_frontend_logical_console(run16_native_frontend *,HANDLE *);
DWORD run16_native_frontend_project_text(run16_native_frontend *);
/* All require the active channel's I/O lock. Clone/commit consume only local
 * storage. Commit consumes the handle; projection failure is an explicit
 * terminal-channel failure after commit, never a rollback to freed storage. */
DWORD run16_native_frontend_clone_text(run16_native_frontend *,HANDLE *,SMALL_RECT *);
DWORD run16_native_frontend_commit_text(run16_native_frontend *,HANDLE,SMALL_RECT);
/* Frontend-local borrowed state; channel access requires the shared I/O lock. */
SMALL_RECT *run16_native_frontend_text_region(run16_native_frontend *);
/* Frontend-local presentation request; never changes target execution. */
DWORD run16_native_frontend_display(run16_native_frontend *,BOOL window);
void run16_native_frontend_console_title_changed(run16_native_frontend *);
/* Called with the frontend I/O lock held by this authenticated channel. */
void run16_native_frontend_worker_title(run16_native_frontend *,const void *,const char *);
/* Terminal teardown must restore the caller's active buffer and input mode
 * before the root launcher returns control to its parent Console. */
DWORD run16_native_frontend_destroy(run16_native_frontend *);
void run16_native_frontend_cancel(run16_native_frontend *);
/* Return the borrowed visible Console to its caller without closing resident
 * worker channels. A later worker activation may reuse the same frontend. */
DWORD run16_native_frontend_park(run16_native_frontend *);
/* One channel owner; acquisition has no worker/device classification. */
DWORD run16_native_frontend_bind(run16_native_frontend *,const void *,BOOL);
/* Active owner requests exact text storage conversion under the I/O lock. */
/* Optional commit result distinguishes rollback from terminal projection error. */
DWORD run16_native_frontend_prepare_text(run16_native_frontend *,const void *,COORD,BOOL *);
/* Wait for this root's actual I/O ownership transition, not for input data.
 * The caller retries the original bind after a successful wait. */
DWORD run16_native_frontend_wait_ready(run16_native_frontend *,const void *,HANDLE,HANDLE,DWORD);
void run16_native_frontend_cancel_pending(run16_native_frontend *,const void *);
/* Channel video is read only under the shared I/O lock. Forget/unbind must
 * detach it before channel storage is disposed. No pointer crosses IPC. */
DWORD run16_native_frontend_video(run16_native_frontend *,const void *,run16_console_video *,BOOL import_text);
/* Complete batch grid/frame/font publication uses the same transaction.
 * Surface is consumed iff committed is TRUE, including projection failure. */
DWORD run16_native_frontend_publish_text(run16_native_frontend *,const void *,
    run16_console_video *,HANDLE,SMALL_RECT,BOOL *committed);
/* Successful enter retains the shared I/O lock until leave. */
DWORD run16_native_frontend_enter(run16_native_frontend *,const void *);
void run16_native_frontend_leave(run16_native_frontend *);
void run16_native_frontend_snapshot_begin(run16_native_frontend *);
void run16_native_frontend_snapshot_end(run16_native_frontend *);
/* Caller holds the successful enter lock. */
BOOL run16_native_frontend_text_frame_required(run16_native_frontend *);
/* Caller holds the frontend I/O lock through console dispatch enter/leave. */
BOOL run16_native_frontend_window_clip_owned(run16_native_frontend *);
/* Caller holds the active channel's I/O lock; copied data only. */
DWORD run16_native_frontend_read_text_configuration(run16_native_frontend *,DWORD,DWORD,console_io_reply *);
/* Borrowed readiness; data operations require the successful enter lock. */
HANDLE run16_native_frontend_ready(run16_native_frontend *);
DWORD run16_native_frontend_read(run16_native_frontend *,BOOL,INPUT_RECORD *,DWORD,DWORD *);
DWORD run16_native_frontend_prepend(run16_native_frontend *,const INPUT_RECORD *,DWORD);
void run16_native_frontend_forget(run16_native_frontend *,const void *);
#endif
