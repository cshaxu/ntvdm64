/* Original ntcsrsrv.h uses NtCurrentTeb()->CsrClientThread. Modern standalone
 * dispatch cannot use NT4's private TEB field or import the CSR thread shell.
 * Preserve thread-local lookup with a borrowed, explicitly bound local record.
 * Caller authenticates first and restores the previous binding on every exit. */
#include "compat/include/base_request.h"

static __declspec(thread) struct _CSR_THREAD *requestThread;

struct _CSR_THREAD *OpenNtBaseServerRequestThread(void)
{
    return requestThread;
}

struct _CSR_THREAD *OpenNtBaseBindServerRequestThread(struct _CSR_THREAD *thread)
{
    struct _CSR_THREAD *previous = requestThread;
    requestThread = thread;
    return previous;
}

static __declspec(thread) const void *requestMessage;
const void *OpenNtBaseServerRequestMessage(void)
{
    return requestMessage;
}
const void *OpenNtBaseBindServerRequestMessage(const void *message)
{
    const void *previous=requestMessage;
    requestMessage=message;
    return previous;
}
