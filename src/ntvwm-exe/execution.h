#ifndef NTVWM_EXECUTION_H
#define NTVWM_EXECUTION_H
#include <windows.h>
#include "next_command.h"

typedef struct ntvwm_executions ntvwm_executions;
typedef struct ntvwm_execution_io {
    void *context;
    DWORD (*begin)(void *,HANDLE stop);
    DWORD (*end)(void *,DWORD request);
    /* Paired with successful begin after the launch attempt, before reply I/O.
     * Lets the Console owner serialize CreateProcess against explicit close. */
    void (*release_launch)(void *);
    /* Completes the temporary I/O resume admission, retaining parent I/O. */
    DWORD (*resume)(void *);
} ntvwm_execution_io;
typedef void (*ntvwm_execution_fault)(void *,DWORD);
DWORD ntvwm_executions_open(ntvwm_executions **);
/* Configure before accepting any request. The callback context outlives close. */
void ntvwm_executions_bind_io(ntvwm_executions *,const ntvwm_execution_io *);
/* A failed broker completion is fatal even while GetNext is blocked. */
void ntvwm_executions_bind_fault(ntvwm_executions *,ntvwm_execution_fault,void *);
BOOL ntvwm_executions_idle(ntvwm_executions *);
/* Drain helper for shutdown and tests; the production GetNext loop must not
 * wait for idle because an active CMD may itself start another run16. */
DWORD ntvwm_executions_wait_idle(ntvwm_executions *);
/* A broker completion failure is a worker fault, not permission to accept a
 * second command while NTSRV may still retain the first record. */
void ntvwm_executions_note_broker_failure(ntvwm_executions *,DWORD);
/* Successful thread handoff consumes the copied command and attachments;
 * on startup failure the caller retains them for completion and disposal.
 * A preflight_error is returned through the authenticated startup RPC,
 * before completion; it is not a broker
 * completion without a peer-visible result.  No process-tree ownership:
 * closing a request never kills its running target. */
DWORD ntvwm_execution_start(ntvwm_executions *,ntvwm_next_command *,DWORD preflight_error);
/* Stop accepting before close. Cancels local waits and joins request cleanup,
 * never native targets. Finished requests release themselves without waiting
 * for another launcher to wake the worker's blocking receive. */
void ntvwm_executions_close(ntvwm_executions *);
#endif
