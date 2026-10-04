#ifndef FRONTEND_SESSION_H
#define FRONTEND_SESSION_H
#include <windows.h>
#include "console_video.h"
#include "common/protocol/console_io.h"
typedef struct frontend_session frontend_session;
DWORD frontend_session_create(frontend_session **);
/* Caller owns non-inheritable duplicates of the session's canonical handles. */
DWORD frontend_session_console(frontend_session *,HANDLE *,HANDLE *);
/* Both worker kinds use the frontend's private logical Console cell grid. */
DWORD frontend_session_logical_console(frontend_session *,HANDLE *);
DWORD frontend_session_project_text(frontend_session *);
/* All require the active channel's I/O lock. Clone/commit consume only local
 * storage. Commit consumes the handle; projection failure is an explicit
 * terminal-channel failure after commit, never a rollback to freed storage. */
DWORD frontend_session_clone_text(frontend_session *,HANDLE *,SMALL_RECT *);
DWORD frontend_session_commit_text(frontend_session *,HANDLE,SMALL_RECT);
/* Frontend-local borrowed state; channel access requires the shared I/O lock. */
SMALL_RECT *frontend_session_text_region(frontend_session *);
/* Frontend-local presentation request; never changes target execution. */
DWORD frontend_session_display(frontend_session *,BOOL window);
void frontend_session_console_title_changed(frontend_session *);
/* Called with the frontend I/O lock held by this authenticated channel. */
void frontend_session_worker_title(frontend_session *,const void *,const char *);
/* Terminal teardown must restore the caller's active buffer and input mode
 * before the root launcher returns control to its parent Console. */
DWORD frontend_session_destroy(frontend_session *);
void frontend_session_cancel(frontend_session *);
/* Return the borrowed visible Console to its caller without closing resident
 * worker channels. A later worker activation may reuse the same frontend. */
DWORD frontend_session_park(frontend_session *);
/* One channel owner; acquisition has no worker/device classification. */
DWORD frontend_session_bind(frontend_session *,const void *,BOOL);
/* Active owner requests exact text storage conversion under the I/O lock. */
/* Optional commit result distinguishes rollback from terminal projection error. */
DWORD frontend_session_prepare_text(frontend_session *,const void *,COORD,BOOL *);
/* Channel video is read only under the shared I/O lock. Forget/unbind must
 * detach it before channel storage is disposed. No pointer crosses IPC. */
DWORD frontend_session_video(frontend_session *,const void *,frontend_video *,BOOL import_text);
/* Complete batch grid/frame/font publication uses the same transaction.
 * Surface is consumed iff committed is TRUE, including projection failure. */
DWORD frontend_session_publish_text(frontend_session *,const void *,
    frontend_video *,HANDLE,SMALL_RECT,BOOL *committed);
/* Successful enter retains the shared I/O lock until leave. */
DWORD frontend_session_enter(frontend_session *,const void *);
void frontend_session_leave(frontend_session *);
void frontend_session_snapshot_begin(frontend_session *);
void frontend_session_snapshot_end(frontend_session *);
/* Caller holds the successful enter lock. */
BOOL frontend_session_text_frame_required(frontend_session *);
/* Caller holds the frontend I/O lock through console dispatch enter/leave. */
BOOL frontend_session_window_clip_owned(frontend_session *);
/* Caller holds the active channel's I/O lock; copied data only. */
DWORD frontend_session_read_text_configuration(frontend_session *,DWORD,DWORD,console_io_reply *);
/* Borrowed readiness; data operations require the successful enter lock. */
HANDLE frontend_session_ready(frontend_session *);
DWORD frontend_session_read(frontend_session *,BOOL,INPUT_RECORD *,DWORD,DWORD *);
DWORD frontend_session_prepend(frontend_session *,const INPUT_RECORD *,DWORD);
void frontend_session_forget(frontend_session *,const void *);
#endif
