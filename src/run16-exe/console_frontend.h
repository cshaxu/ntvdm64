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
} run16_console_frontend;
DWORD run16_console_dispatch(run16_console_frontend *,const console_io_request *,
    console_io_reply *);
#endif
