#ifndef RUN16_CONSOLE_VIDEO_H
#define RUN16_CONSOLE_VIDEO_H
#include <windows.h>
#include "common/protocol/console_video.h"

/* Access is serialized by the channel owner, including disposal. Rendering
 * must take a snapshot through that owner; raw pointers never cross IPC. */
typedef struct run16_console_video {
    console_video_description pending_description, description;
    BYTE *pending, *pixels;
    uint32_t serial, pending_serial, received, published_serial;
    BOOL pending_validated;
    console_text_configuration configuration;
    uint32_t configuration_serial;
} run16_console_video;
DWORD run16_console_video_begin(run16_console_video *, uint32_t,
    const console_video_description *);
DWORD run16_console_video_data(run16_console_video *, uint32_t, uint32_t,
    const void *, uint32_t);
/* Receive/validate without replacing committed storage. Complete staging is
 * committed only after the owning frontend has prepared its logical grid. */
DWORD run16_console_video_stage_data(run16_console_video *,uint32_t,uint32_t,const void *,uint32_t);
DWORD run16_console_video_commit_pending(run16_console_video *);
void run16_console_video_abort_pending(run16_console_video *);
DWORD run16_console_video_text(run16_console_video *, uint32_t);
void run16_console_video_dispose(run16_console_video *);
#endif
