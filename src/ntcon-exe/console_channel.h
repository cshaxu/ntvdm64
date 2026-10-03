#ifndef RUN16_CONSOLE_CHANNEL_H
#define RUN16_CONSOLE_CHANNEL_H
#include <windows.h>
#include "native_console_frontend.h"
typedef struct run16_console_channel run16_console_channel;
/* The authenticated request query supplied worker. This call consumes that
 * local process reference on every outcome, including allocation failure. */
DWORD run16_console_channel_start_request(DWORD request,HANDLE worker,run16_native_frontend *,run16_console_channel **);
HANDLE run16_console_channel_thread(run16_console_channel *);
/* On timeout the channel remains owned and must not be freed with its root. */
DWORD run16_console_channel_stop(run16_console_channel *);
#endif
