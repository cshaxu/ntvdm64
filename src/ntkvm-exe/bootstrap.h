#ifndef FRONTEND_BOOTSTRAP_H
#define FRONTEND_BOOTSTRAP_H
#include <windows.h>
#include <stdint.h>
#include "product-abi/version.h"
#define FRONTEND_BOOTSTRAP_VERSION 2u
typedef struct frontend_bootstrap_reply {
    uint32_t version,status;
    char application[APP_VERSION_BYTES];
} frontend_bootstrap_reply;
/* Exact created process and its authenticated inherited channel, not a PID token. */
typedef struct frontend_connection { HANDLE process,channel,capability; } frontend_connection;
DWORD frontend_bootstrap_start(PCWSTR,frontend_connection *);
/* Release local references only; never terminates the frontend or its targets. */
void frontend_bootstrap_release(frontend_connection *);
#endif
