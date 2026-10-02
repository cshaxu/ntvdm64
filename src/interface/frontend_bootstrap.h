#ifndef FRONTEND_BOOTSTRAP_H
#define FRONTEND_BOOTSTRAP_H
#include <windows.h>
#include <stdint.h>
#include "interface/frontend_protocol.h"
/* Local references returned by NTSRV's authenticated StartFrontend RPC.
 * No direct bootstrap channel or launcher-owned retirement signal. */
typedef struct frontend_connection { HANDLE process,capability,restored; } frontend_connection;
/* Release local references only; never terminates the frontend or its targets. */
void frontend_bootstrap_release(frontend_connection *);
#endif
