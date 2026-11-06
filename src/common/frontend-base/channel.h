#ifndef COMMON_FRONTEND_BASE_CHANNEL_H
#define COMMON_FRONTEND_BASE_CHANNEL_H
#include <windows.h>
#include "common/protocol/console_io.h"

typedef struct frontend_base_channel frontend_base_channel;
/* Borrowed until dispatch returns. Providers that block must include these
 * handles in their wait set; pipe cancellation cannot cancel a provider wait. */
typedef struct frontend_base_cancel {
    HANDLE stop,worker;
} frontend_base_cancel;
typedef struct frontend_base_result {
    BOOL reply;
    DWORD terminal_error; /* Close after the reply, if one was requested. */
} frontend_base_result;
typedef struct frontend_base_channel_ops {
    DWORD (*bind)(void *);
    DWORD (*attach)(void *,DWORD,HANDLE,HANDLE,DWORD *);
    void (*generation)(void *,DWORD);
    BOOL (*active)(void *);
    DWORD (*dispatch)(void *,const frontend_base_cancel *,const console_io_request *,console_io_reply *,frontend_base_result *);
    DWORD (*input_pending)(void *,BOOL *);
    void (*finish)(void *); /* Serving thread, before endpoint EOF. */
} frontend_base_channel_ops;

/* Consumes worker on every outcome. Context and input_ready are borrowed
 * until stop joins, including failure cleanup. Callbacks are process-local. */
DWORD frontend_base_channel_start(DWORD request,HANDLE worker,HANDLE input_ready,
    const frontend_base_channel_ops *,void *context,frontend_base_channel **);
HANDLE frontend_base_channel_thread(frontend_base_channel *);
HANDLE frontend_base_channel_ready(frontend_base_channel *);
/* Failure retains the object and all resources still borrowed by its thread. */
DWORD frontend_base_channel_stop(frontend_base_channel *);
#endif
