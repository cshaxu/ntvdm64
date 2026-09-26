#ifndef RUN16_FRONTEND_SCOPE_H
#define RUN16_FRONTEND_SCOPE_H
#include <windows.h>
typedef struct run16_frontend_scope run16_frontend_scope;
/* Requires the caller's connected BaseClient. End before disconnecting it. */
DWORD run16_frontend_scope_begin(run16_frontend_scope **,void (*channel_ready)(void));
void run16_frontend_scope_end(run16_frontend_scope *);
HANDLE run16_frontend_scope_capability(run16_frontend_scope *);
#endif
