#ifndef COMPAT_CONSOLE_ENDPOINT_H
#define COMPAT_CONSOLE_ENDPOINT_H

#include "console_transport.h"

/* A single ordered request stream and lock, shared with the product owner. */
typedef struct compat_console_endpoint {
    ntcon_worker_client channel;
    CRITICAL_SECTION lock;
    console_io_request request;
} compat_console_endpoint;

BOOL compat_console_state_operation(compat_console_endpoint *endpoint, DWORD operation,
    const console_io_state *state, console_io_state *result);

#endif
