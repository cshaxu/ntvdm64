#include "mvdm_softpc_ccpu_wait.h"

/* The worker creates this before TimerInit can start an interrupt producer.
 * It is closed only after the original host has stopped those producers and
 * the sole CCPU execution thread has returned from its HLT wait. */
static HANDLE ccpu_wake_event;

BOOL mvdm_softpc_ccpu_wait_begin(void)
{
    if (ccpu_wake_event != NULL)
        return TRUE;

    ccpu_wake_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    return ccpu_wake_event != NULL;
}

void mvdm_softpc_ccpu_wait_end(void)
{
    HANDLE event = ccpu_wake_event;

    ccpu_wake_event = NULL;
    if (event != NULL)
        CloseHandle(event);
}

void mvdm_softpc_ccpu_wait_signal(void)
{
    if (ccpu_wake_event != NULL)
        SetEvent(ccpu_wake_event);
}

void mvdm_softpc_ccpu_wait_for_event(void)
{
    if (ccpu_wake_event != NULL)
        (void)WaitForSingleObject(ccpu_wake_event, INFINITE);
    else
        Sleep(0);
}
