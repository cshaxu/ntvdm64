#include "session_cancellation.h"

#include <intrin.h>

static volatile long active = 0;
static volatile long reason = 0;
#if defined(RUNTIME_CANCELLATION_TESTING)
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

int app_session_cancellation_activate(void)
{
    store(&reason, APP_SESSION_CANCELLATION_NONE);
#if defined(RUNTIME_CANCELLATION_TESTING)
    store(&test_polls, 0u);
#endif
    return (uint32_t)_InterlockedCompareExchange(&active, 1, 0) == 0u;
}

void app_session_cancellation_deactivate(void)
{
    store(&active, 0u);
    store(&reason, APP_SESSION_CANCELLATION_NONE);
}

int app_session_cancellation_request(uint32_t input)
{
    uint32_t expected = APP_SESSION_CANCELLATION_NONE;
    if (input != APP_SESSION_CANCELLATION_USER_REQUEST ||
        load(&active) == 0u ||
        (uint32_t)_InterlockedCompareExchange(&reason, (long)input,
            (long)expected) != expected) return 0;
    return load(&active) != 0u;
}

uint32_t app_session_cancellation_requested_reason(void)
{
    return load(&active) == 0u ? APP_SESSION_CANCELLATION_NONE :
        load(&reason);
}

#if defined(RUNTIME_CANCELLATION_TESTING)
void app_session_cancellation_test_poll_mark(void)
{
    _InterlockedIncrement(&test_polls);
}

uint32_t app_session_cancellation_test_poll_count(void)
{
    return load(&test_polls);
}
#endif
