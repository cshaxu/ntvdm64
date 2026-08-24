#include "ntdos64_session_cancellation_v1.h"

#include <intrin.h>

static volatile long active = 0;
static volatile long reason = 0;
#if defined(BX_NTVDM_CANCELLATION_TESTING)
static volatile long test_polls = 0;
#endif

static uint32_t load(volatile long *value)
{
    return (uint32_t)_InterlockedCompareExchange(value, 0, 0);
}

static void store(volatile long *value, uint32_t input)
{
    _InterlockedExchange(value, (long)input);
}

int ntdos64_session_cancellation_v1_activate(void)
{
    store(&reason, NTDOS64_SESSION_CANCELLATION_V1_NONE);
#if defined(BX_NTVDM_CANCELLATION_TESTING)
    store(&test_polls, 0u);
#endif
    return (uint32_t)_InterlockedCompareExchange(&active, 1, 0) == 0u;
}

void ntdos64_session_cancellation_v1_deactivate(void)
{
    store(&active, 0u);
    store(&reason, NTDOS64_SESSION_CANCELLATION_V1_NONE);
}

int ntdos64_session_cancellation_v1_request(uint32_t input)
{
    uint32_t expected = NTDOS64_SESSION_CANCELLATION_V1_NONE;
    if (input != NTDOS64_SESSION_CANCELLATION_V1_USER_REQUEST ||
        load(&active) == 0u ||
        (uint32_t)_InterlockedCompareExchange(&reason, (long)input,
            (long)expected) != expected) return 0;
    return load(&active) != 0u;
}

uint32_t ntdos64_session_cancellation_v1_requested_reason(void)
{
    return load(&active) == 0u ? NTDOS64_SESSION_CANCELLATION_V1_NONE :
        load(&reason);
}

#if defined(BX_NTVDM_CANCELLATION_TESTING)
void ntdos64_session_cancellation_v1_test_poll_mark(void)
{
    _InterlockedIncrement(&test_polls);
}

uint32_t ntdos64_session_cancellation_v1_test_poll_count(void)
{
    return load(&test_polls);
}
#endif
