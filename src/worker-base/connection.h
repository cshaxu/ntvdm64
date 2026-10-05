#ifndef WORKER_BASE_CONNECTION_H
#define WORKER_BASE_CONNECTION_H
#include <windows.h>
/* Worker-side broker lifetime binding. The caller owns CsrPortHeap and its
 * backend state; it must disconnect before destroying that storage. */
DWORD worker_base_connect(void);
/* Caller supplies its already classified execution route. Window-only work
 * skips character startup locally; classification and execution stay local. */
static __inline DWORD worker_base_start_character_io(BOOL required,
    DWORD (*begin)(void *context,HANDLE stop),void *context,HANDLE stop)
{
    if(!required)return ERROR_SUCCESS;
    if(!begin)return ERROR_INVALID_PARAMETER;
    return begin(context,stop);
}
/* Caller owns a synchronize-only shutdown event; NTSRV alone signals it. */
DWORD worker_base_shutdown_event(HANDLE *shutdown);
/* Execute worker-local close after an authenticated instruction. Grace is
 * supplied by that existing owner (INFINITE calls inline). A successful close
 * alone may signal closed; failure never fabricates that acknowledgment.
 * Always ends this carrier only, not a process tree. Does not return. */
void worker_base_shutdown_close(LPTHREAD_START_ROUTINE close,void *context,
    DWORD grace,DWORD exit_code,HANDLE closed);
/* Service instruction to publish/return input and release presentation.
 * This does not end a task or worker. Caller owns a wait-only event handle. */
DWORD worker_base_io_release_event(HANDLE *release);
/* Report execution facts. Only the authenticated service response decides
 * whether this endpoint remains usable or must perform final-I/O release. */
DWORD worker_base_io_checkpoint(DWORD reason,DWORD request,DWORD *decision);
/* Caller serializes its endpoint and quiesces all pipe calls before close.
 * Peer handles authenticate transport only; NTSRV owns the association.
 * Handles are explicit instance state; NULL means no physical channel. */
DWORD worker_base_io_open(HANDLE *pipe,HANDLE *peer,HANDLE *ready,DWORD *generation);
DWORD worker_base_io_close(HANDLE *pipe,HANDLE *peer,HANDLE *ready);
void worker_base_disconnect(void);
#endif
