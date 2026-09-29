#ifndef RUN16_FRONTEND_SCOPE_H
#define RUN16_FRONTEND_SCOPE_H
#include <windows.h>
#include "interface/native_launch.h"
typedef struct run16_frontend_scope run16_frontend_scope;
/* Requires the caller's connected BaseClient. End before disconnecting it. */
DWORD run16_frontend_scope_begin(run16_frontend_scope **);
void run16_frontend_scope_end(run16_frontend_scope *);
HANDLE run16_frontend_scope_capability(run16_frontend_scope *);
BOOL run16_frontend_scope_has_execution(run16_frontend_scope *);
DWORD run16_frontend_scope_console_mask(run16_frontend_scope *);
DWORD run16_frontend_scope_launch_native(run16_frontend_scope *,const run16_native_start *,HANDLE *);
DWORD run16_frontend_scope_wait_native(run16_frontend_scope *,HANDLE,DWORD *);
DWORD run16_frontend_scope_resume_parent(run16_frontend_scope *);
#endif
