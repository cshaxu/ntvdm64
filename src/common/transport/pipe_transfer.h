#ifndef COMMON_PIPE_TRANSFER_H
#define COMMON_PIPE_TRANSFER_H
#include <windows.h>

/* Borrowed pipe/event and caller buffer. A pending operation must be finished
 * or cancelled/drained before any of them is freed. No endpoint policy. */
typedef struct common_pipe_operation {
    OVERLAPPED io;
    HANDLE pipe;
    DWORD requested;
    BOOL pending;
} common_pipe_operation;
typedef enum common_pipe_priority {
    COMMON_PIPE_COMPLETION_FIRST,
    COMMON_PIPE_PEER_DEATH_FIRST
} common_pipe_priority;

DWORD common_pipe_begin(common_pipe_operation *,HANDLE pipe,HANDLE event,
    BOOL write,void *buffer,DWORD bytes,DWORD capacity,DWORD *transferred);
DWORD common_pipe_finish(common_pipe_operation *,DWORD *transferred);
void common_pipe_cancel_drain(common_pipe_operation *);
DWORD common_pipe_transfer(HANDLE pipe,HANDLE peer,HANDLE cancel,HANDLE event,
    common_pipe_priority priority,DWORD peer_error,BOOL write,void *buffer,
    DWORD bytes,DWORD capacity);
#endif
