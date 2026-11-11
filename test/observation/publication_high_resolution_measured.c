/* Measurement-only variant of the production publisher.  It retains the
 * exact one-shot/50Hz/coalescing logic and changes only the timer factory so
 * S1 can quantify scheduler rounding before proposing a production repair. */
#include <windows.h>

static HANDLE measured_create_waitable_timer(LPSECURITY_ATTRIBUTES attributes,
    BOOL manual_reset,LPCWSTR name)
{
    HANDLE timer;
    (void)manual_reset;
    timer=CreateWaitableTimerExW(attributes,name,CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_ALL_ACCESS);
    return timer ? timer : CreateWaitableTimerW(attributes,FALSE,name);
}

#define CreateWaitableTimerW measured_create_waitable_timer
#include "../../src/worker-base/publication.c"
