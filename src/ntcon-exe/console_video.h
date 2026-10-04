#ifndef FRONTEND_VIDEO_H
#define FRONTEND_VIDEO_H
#include <windows.h>
#include "common/protocol/console_video.h"

/* Access is serialized by the channel owner, including disposal. Rendering
 * must take a snapshot through that owner; raw pointers never cross IPC. */
typedef struct frontend_video {
    console_video_description pending_description, description;
    BYTE *pending, *pixels;
    uint32_t serial, pending_serial, received, published_serial;
    BOOL pending_validated;
    console_text_configuration configuration;
    uint32_t configuration_serial;
} frontend_video;
DWORD frontend_video_begin(frontend_video *, uint32_t,
    const console_video_description *);
DWORD frontend_video_data(frontend_video *, uint32_t, uint32_t,
    const void *, uint32_t);
/* Receive/validate without replacing committed storage. Complete staging is
 * committed only after the owning frontend has prepared its logical grid. */
DWORD frontend_video_stage_data(frontend_video *,uint32_t,uint32_t,const void *,uint32_t);
DWORD frontend_video_commit_pending(frontend_video *);
void frontend_video_abort_pending(frontend_video *);
DWORD frontend_video_text(frontend_video *, uint32_t);
void frontend_video_dispose(frontend_video *);
#endif
