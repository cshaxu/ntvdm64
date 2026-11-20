#include "include/input_readiness.h"

DWORD compat_input_readiness_ack(compat_input_readiness *readiness)
{
    DWORD error;
    if (!readiness) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&readiness->lock);
    error = readiness->error;
    if (!error && (!ResetEvent(readiness->wake) || !SetEvent(readiness->rearm)))
        error = GetLastError();
    LeaveCriticalSection(&readiness->lock);
    return error;
}
