#ifndef RUN16_NATIVE_CONSOLE_REQUEST_H
#define RUN16_NATIVE_CONSOLE_REQUEST_H
#include "native_console_frontend.h"
#include "native_request_client.h"
typedef struct run16_native_request run16_native_request;
/* Root consumes all three broker-delivered handles, including on failure. */
DWORD run16_native_request_start(run16_native_frontend *,HANDLE root_capability,HANDLE stop,
    HANDLE channel,HANDLE sender,HANDLE execution,run16_native_request **);
HANDLE run16_native_request_thread(run16_native_request *);
void run16_native_request_close(run16_native_request *);
#endif
