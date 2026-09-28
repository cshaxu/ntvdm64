#ifndef RUN16_CONSOLE_FRONTEND_H
#define RUN16_CONSOLE_FRONTEND_H
#include <windows.h>
#include "product-abi/console_io.h"
#include "console_video.h"

/* One explicit frontend instance. These handles never enter the wire. */
typedef struct run16_console_frontend {
    HANDLE input,output;
    uint32_t generation,sequence;
    run16_console_video video;
    void *io_context;
    DWORD (*activate)(void *,BOOL);
    DWORD (*enter)(void *);
    void (*leave)(void *);
    /* Frontend-local shared-screen transaction; not a worker/IPC callback. */
    DWORD (*screen_begin)(void *);
    DWORD (*screen_end)(void *,BOOL);
    /* Called while enter's I/O lock is held; policy only, no UI mutation. */
    BOOL (*text_frame_required)(void *);
    /* Copied input owned by frontend; callbacks hold the enter lock. */
    DWORD (*read_input)(void *,BOOL,INPUT_RECORD *,DWORD,DWORD *);
    DWORD (*prepend_input)(void *,const INPUT_RECORD *,DWORD);
} run16_console_frontend;
DWORD run16_console_dispatch(run16_console_frontend *,const console_io_request *,
    console_io_reply *);
#endif
