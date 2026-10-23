#ifndef RUN16_FRONTEND_SCOPE_H
#define RUN16_FRONTEND_SCOPE_H
#include <windows.h>
#include "common/codec/native_launch.h"
#include "common/protocol/native_launch_type.h"
typedef struct run16_frontend_scope run16_frontend_scope;
/* Requires the caller's connected BaseClient. End before disconnecting it. */
DWORD run16_frontend_scope_begin(run16_frontend_scope **);
DWORD run16_frontend_scope_begin_lease(run16_frontend_scope **,BOOL console_owned);
/* Authenticate inherited context only; never acquire a text frontend. */
DWORD run16_frontend_scope_begin_gui(run16_frontend_scope **);
void run16_frontend_scope_end(run16_frontend_scope *);
HANDLE run16_frontend_scope_capability(run16_frontend_scope *);
BOOL run16_frontend_scope_has_execution(run16_frontend_scope *);
DWORD run16_frontend_scope_console_mask(run16_frontend_scope *);
DWORD run16_frontend_scope_creation_flags(run16_frontend_scope *);
DWORD run16_frontend_scope_launch_native(run16_frontend_scope *,const run16_native_start *,DWORD native_type);
DWORD run16_frontend_scope_wait_native(run16_frontend_scope *,DWORD *,DWORD *);
/* Common direct-task receipt wait; result decoding remains with its source. */
DWORD run16_wait_direct_event(HANDLE receipt);
/* Complete the admitted request before releasing its client connection.
 * NTSRV decides whether completion includes an outer Console lease return. */
DWORD run16_frontend_scope_finish(run16_frontend_scope *);
#endif
