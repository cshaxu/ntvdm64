#ifndef RUN16_NATIVE_CONSOLE_FRONTEND_H
#define RUN16_NATIVE_CONSOLE_FRONTEND_H
#include "native_console_backend.h"
typedef struct run16_native_frontend run16_native_frontend;
DWORD run16_native_frontend_create(run16_native_frontend **);
DWORD run16_native_frontend_launch(run16_native_frontend *,const run16_native_start *,HANDLE *);
DWORD run16_native_frontend_wait(run16_native_frontend *,HANDLE,DWORD *);
void run16_native_frontend_destroy(run16_native_frontend *);
void run16_native_frontend_cancel(run16_native_frontend *);
DWORD run16_native_frontend_members(run16_native_frontend *,DWORD *);
DWORD run16_native_frontend_drain(run16_native_frontend *);
DWORD run16_native_frontend_dos_bind(run16_native_frontend *,const void *,BOOL);
/* Successful enter retains the shared I/O lock until leave. */
DWORD run16_native_frontend_dos_enter(run16_native_frontend *,const void *);
void run16_native_frontend_dos_leave(run16_native_frontend *);
void run16_native_frontend_dos_forget(run16_native_frontend *,const void *);
#endif
