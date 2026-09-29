#ifndef FRONTEND_BOOTSTRAP_H
#define FRONTEND_BOOTSTRAP_H
#include <windows.h>
#include <stdint.h>
#include "interface/frontend_protocol.h"
/* Exact created process and its authenticated inherited channel, not a PID token. */
typedef struct frontend_connection { HANDLE process,channel,capability; } frontend_connection;
DWORD frontend_bootstrap_start(PCWSTR,frontend_connection *);
/* Release local references only; never terminates the frontend or its targets. */
void frontend_bootstrap_release(frontend_connection *);
#endif
