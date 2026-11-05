#ifndef COMMON_FRONTEND_BASE_SERVICE_H
#define COMMON_FRONTEND_BASE_SERVICE_H
#include <windows.h>
typedef struct frontend_base_service frontend_base_service;
typedef struct frontend_base_service_ops {
    DWORD (*retirement)(void *,DWORD *);
    DWORD (*prepare_requests)(void *);
    DWORD (*request)(void *,DWORD *,HANDLE *);
    /* start consumes the authenticated worker on every outcome. */
    DWORD (*channel_start)(void *,DWORD,HANDLE,void **);
    DWORD (*channel_stop)(void *,void *);
    HANDLE (*channel_thread)(void *,void *);
    DWORD (*disconnected)(void *);
    DWORD (*fault)(void *);
} frontend_base_service_ops;
/* Event/process references and context are borrowed until successful stop.
 * No process-global client or presentation state belongs to this instance. */
DWORD frontend_base_service_start(HANDLE notification,HANDLE retire,HANDLE broker,
    HANDLE state_changed,const frontend_base_service_ops *,void *,frontend_base_service **);
HANDLE frontend_base_service_thread(frontend_base_service *);
/* Retains object and borrowed resources if either service/channel join fails. */
DWORD frontend_base_service_stop(frontend_base_service *);
#endif
