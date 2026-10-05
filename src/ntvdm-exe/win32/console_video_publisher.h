#ifndef NTVDM_CONSOLE_VIDEO_PUBLISHER_H
#define NTVDM_CONSOLE_VIDEO_PUBLISHER_H
#include <windows.h>
#include "common/protocol/console_video.h"

/* NTVDM-owned presentation cadence, not a guest timer or frontend policy.
 * Caller supplies immutable complete copies, never VGA/CPU/painter pointers. */
typedef struct ntvdm_video_publisher ntvdm_video_publisher;
typedef DWORD (*ntvdm_video_send_fn)(void *,const console_video_description *,const void *);
ntvdm_video_publisher *ntvdm_video_publisher_create(ntvdm_video_send_fn,void *,HANDLE);
DWORD ntvdm_video_publisher_offer(ntvdm_video_publisher *,const console_video_description *,const void *,BOOL *);
DWORD ntvdm_video_publisher_active(ntvdm_video_publisher *,BOOL);
void ntvdm_video_publisher_destroy(ntvdm_video_publisher *);
#endif
