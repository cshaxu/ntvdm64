#ifndef RUN16_CONSOLE_FRONTEND_H
#define RUN16_CONSOLE_FRONTEND_H
#include <windows.h>
#include "interface/console_io.h"
#include "console_video.h"

/* One explicit frontend instance. These handles never enter the wire. */
typedef struct run16_console_frontend {
    HANDLE input,output;
    uint32_t generation,sequence;
    /* Borrowed session geometry, protected by enter/leave. Physical Console
     * clipping is presentation only and must not become worker geometry. */
    SMALL_RECT *logical_window;
    run16_console_video video;
    void *io_context;
    DWORD (*activate)(void *,BOOL,DWORD);
    DWORD (*enter)(void *);
    void (*leave)(void *);
    /* Frontend-local shared-screen transaction; not a worker/IPC callback. */
    DWORD (*screen_begin)(void *);
    DWORD (*screen_end)(void *,BOOL);
    DWORD (*snapshot_begin)(void *);
    DWORD (*snapshot_end)(void *);
    /* Called while enter's I/O lock is held; policy only, no UI mutation. */
    BOOL (*text_frame_required)(void *);
    /* The Window presentation, not DOS, owns the host pointer clip. */
    BOOL (*window_clip_owned)(void *);
    /* A successful DOS Console title change wakes NTKVM presentation. */
    void (*title_changed)(void *);
    /* Copied caption metadata, not a SetConsoleTitle on the root Console. */
    void (*publish_title)(void *,const char *);
    /* Copied input owned by frontend; callbacks hold the enter lock. */
    DWORD (*read_input)(void *,BOOL,INPUT_RECORD *,DWORD,DWORD *);
    DWORD (*prepend_input)(void *,const INPUT_RECORD *,DWORD);
    DWORD (*read_text_configuration)(void *,DWORD,DWORD,console_io_reply *);
} run16_console_frontend;
DWORD run16_console_dispatch(run16_console_frontend *,const console_io_request *,
    console_io_reply *);
/* Original nt_fulsc.c::calcScreenParams can reproduce these return modes.
 * Caller serializes the screen and commits ownership only after success. */
BOOL run16_console_dos_size(COORD);
DWORD run16_console_prepare_dos(HANDLE,SMALL_RECT *);
#endif
