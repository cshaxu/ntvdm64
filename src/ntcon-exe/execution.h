#ifndef NTCON_EXECUTION_H
#define NTCON_EXECUTION_H
#include <windows.h>
#include "worker-base/next_command.h"

typedef struct ntcon_executions ntcon_executions;
typedef struct ntcon_execution_io {
    void *context;
    DWORD (*begin)(void *,HANDLE stop);
    DWORD (*end)(void *);
    /* Paired with successful begin after the launch attempt, before reply I/O.
     * Lets the Console owner serialize CreateProcess against explicit close. */
    void (*release_launch)(void *);
} ntcon_execution_io;
typedef void (*ntcon_execution_fault)(void *,DWORD);
DWORD ntcon_executions_open(ntcon_executions **);
/* Configure before accepting any request. The callback context outlives close. */
void ntcon_executions_bind_io(ntcon_executions *,const ntcon_execution_io *);
/* A failed broker completion is fatal even while GetNext is blocked. */
void ntcon_executions_bind_fault(ntcon_executions *,ntcon_execution_fault,void *);
BOOL ntcon_executions_idle(ntcon_executions *);
/* Drain helper for shutdown and tests; the production GetNext loop must not
 * wait for idle because an active CMD may itself start another run16. */
DWORD ntcon_executions_wait_idle(ntcon_executions *);
/* A broker completion failure is a worker fault, not permission to accept a
 * second command while NTSRV may still retain the first record. */
void ntcon_executions_note_broker_failure(ntcon_executions *,DWORD);
/* Consumes all authenticated command attachments, including on failure.  A
 * preflight_error is returned through the normal native request reply after
 * the requester has finished its header/payload transfer; it is not a broker
 * completion without a peer-visible result.  No process-tree ownership:
 * closing a request never kills its running target. */
DWORD ntcon_execution_start(ntcon_executions *,worker_base_next_command *,DWORD preflight_error);
/* Stop accepting before close. Cancels local waits and joins request cleanup,
 * never native targets. Finished requests release themselves without waiting
 * for another launcher to wake the worker's blocking receive. */
void ntcon_executions_close(ntcon_executions *);
#endif
