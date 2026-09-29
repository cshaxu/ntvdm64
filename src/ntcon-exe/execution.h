#ifndef NTCON_EXECUTION_H
#define NTCON_EXECUTION_H
#include <windows.h>

typedef struct ntcon_executions ntcon_executions;
typedef struct ntcon_execution_io {
    void *context;
    DWORD (*begin)(void *,HANDLE stop);
    DWORD (*end)(void *);
    /* Paired with successful begin after the launch attempt, before reply I/O.
     * Lets the Console owner serialize CreateProcess against explicit close. */
    void (*release_launch)(void *);
} ntcon_execution_io;
DWORD ntcon_executions_open(ntcon_executions **);
/* Configure before accepting any request. The callback context outlives close. */
void ntcon_executions_bind_io(ntcon_executions *,const ntcon_execution_io *);
BOOL ntcon_executions_idle(ntcon_executions *);
/* Consumes all four authenticated attachments, including on failure. No
 * process-tree ownership: closing a request never kills its running target. */
DWORD ntcon_execution_start(ntcon_executions *,HANDLE frontend,HANDLE channel,
    HANDLE sender,HANDLE execution);
/* Stop accepting before close. Cancels local waits and joins request cleanup,
 * never native targets. Finished requests release themselves without waiting
 * for another launcher to wake the worker's blocking receive. */
void ntcon_executions_close(ntcon_executions *);
#endif
