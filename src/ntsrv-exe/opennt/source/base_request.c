/* Original ntcsrsrv.h uses NtCurrentTeb()->CsrClientThread. Modern standalone
 * dispatch cannot use NT4's private TEB field or import the CSR thread shell.
 * Preserve thread-local lookup with a borrowed, explicitly bound local record.
 * Caller authenticates first and restores the previous binding on every exit. */
#include <base_server.h>

static __declspec(thread) PCSR_THREAD requestThread;

PCSR_THREAD OpenNtBaseServerRequestThread(void)
{
    return requestThread;
}

PCSR_THREAD OpenNtBaseBindServerRequestThread(PCSR_THREAD thread)
{
    PCSR_THREAD previous = requestThread;
    requestThread = thread;
    return previous;
}
