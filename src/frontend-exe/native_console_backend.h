#ifndef RUN16_NATIVE_CONSOLE_BACKEND_H
#define RUN16_NATIVE_CONSOLE_BACKEND_H
#include "native_console_host.h"
typedef struct run16_native_backend run16_native_backend;
/* Root-frontend-owned object. Inner launchers submit to that root, never open
 * a backend themselves. Native target completion is a separate process handle. */
DWORD run16_native_backend_open(PCWSTR image,run16_native_backend **);
DWORD run16_native_backend_open_cancel(PCWSTR image,HANDLE stop,run16_native_backend **);
DWORD run16_native_backend_call(run16_native_backend *,const run16_native_host_request *,
    const void *,run16_native_host_reply *,void *,DWORD);
DWORD run16_native_backend_launch(run16_native_backend *,const run16_native_start *,HANDLE *);
/* Snapshot of attached users, not execution descendants or a completion code.
 * On failure leave the output untouched; never infer an empty session. */
DWORD run16_native_backend_members(run16_native_backend *,DWORD *);
void run16_native_backend_cancel(run16_native_backend *);
HANDLE run16_native_backend_process(run16_native_backend *); /* Borrowed; root only. */
DWORD run16_native_backend_close(run16_native_backend *);
#endif
