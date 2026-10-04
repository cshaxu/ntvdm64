#ifndef FRONTEND_IO_CHANNEL_H
#define FRONTEND_IO_CHANNEL_H
#include <windows.h>
#include "frontend_session.h"
typedef struct frontend_io_channel frontend_io_channel;
/* The authenticated request query supplied worker. This call consumes that
 * local process reference on every outcome, including allocation failure. */
DWORD frontend_io_channel_start_request(DWORD request,HANDLE worker,frontend_session *,frontend_io_channel **);
HANDLE frontend_io_channel_thread(frontend_io_channel *);
/* On timeout the channel remains owned and must not be freed with its root. */
DWORD frontend_io_channel_stop(frontend_io_channel *);
#endif
