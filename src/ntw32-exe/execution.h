#ifndef NTW32_EXECUTION_H
#define NTW32_EXECUTION_H
#include <windows.h>
#include "next_command.h"

typedef struct ntw32_executions ntw32_executions;
typedef struct ntw32_execution_io {
    void *context;
    DWORD (*begin)(void *,HANDLE stop);
    DWORD (*end)(void *);
    /* Paired with successful begin after the launch attempt, before reply I/O.
     * Lets the Console owner serialize CreateProcess against explicit close. */
    void (*release_launch)(void *);
} ntw32_execution_io;
typedef void (*ntw32_execution_fault)(void *,DWORD);
DWORD ntw32_executions_open(ntw32_executions **);
/* Configure before accepting any request. The callback context outlives close. */
void ntw32_executions_bind_io(ntw32_executions *,const ntw32_execution_io *);
/* A failed broker completion is fatal even while GetNext is blocked. */
void ntw32_executions_bind_fault(ntw32_executions *,ntw32_execution_fault,void *);
BOOL ntw32_executions_idle(ntw32_executions *);
/* Drain helper for shutdown and tests; the production GetNext loop must not
 * wait for idle because an active CMD may itself start another run16. */
DWORD ntw32_executions_wait_idle(ntw32_executions *);
/* A broker completion failure is a worker fault, not permission to accept a
 * second command while NTSRV may still retain the first record. */
void ntw32_executions_note_broker_failure(ntw32_executions *,DWORD);
/* Consumes all authenticated command attachments, including on failure.  A
 * preflight_error is returned through the normal native request reply after
 * the requester has finished its header/payload transfer; it is not a broker
 * completion without a peer-visible result.  No process-tree ownership:
 * closing a request never kills its running target. */
DWORD ntw32_execution_start(ntw32_executions *,ntw32_next_command *,DWORD preflight_error);
/* Stop accepting before close. Cancels local waits and joins request cleanup,
 * never native targets. Finished requests release themselves without waiting
 * for another launcher to wake the worker's blocking receive. */
void ntw32_executions_close(ntw32_executions *);
#endif
