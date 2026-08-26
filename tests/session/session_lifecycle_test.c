#include "session/session.h"

static uint32_t teardown_order[2];
static uint32_t teardown_count;

static void record_teardown(void *context)
{
    teardown_order[teardown_count++] = *(const uint32_t *)context;
}

int main(void)
{
    session first;
    session second;
    const uint32_t one = 1u;
    const uint32_t two = 2u;

    session_initialize(&first, 101u);
    session_initialize(&second, 202u);
    if (!session_valid(&first) || !session_valid(&second) ||
        session_activate(&first) == 0 || session_activate(&second) == 0)
        return 1;
    if (session_thread_bind(&first) == 0 || session_thread_current() != &first ||
        session_thread_bind(&second) != 0 || session_thread_unbind(&second) != 0 ||
        session_dispose(&first) != 0 || session_thread_unbind(&first) == 0)
        return 2;
    if (session_register_teardown(&first, record_teardown, (void *)&one) == 0 ||
        session_register_teardown(&first, record_teardown, (void *)&two) == 0 ||
        session_register_teardown(&first, record_teardown, (void *)&one) == 0 ||
        first.teardown_count != 2u)
        return 3;
    session_complete(&second, 37u);
    if (second.state != SESSION_STATE_COMPLETED || second.completion_code != 37u ||
        session_request_cancellation(&second, SESSION_CANCELLATION_REQUESTED) != 0)
        return 4;
    if (session_request_cancellation(&first, SESSION_CANCELLATION_REQUESTED) == 0 ||
        first.state != SESSION_STATE_CANCELLED ||
        first.cancellation_reason != SESSION_CANCELLATION_REQUESTED ||
        session_register_teardown(&first, record_teardown, (void *)&one) != 0)
        return 5;
    if (session_dispose(&first) == 0 || session_valid(&first) ||
        teardown_count != 2u || teardown_order[0] != 2u || teardown_order[1] != 1u ||
        session_dispose(&second) == 0)
        return 6;
    return 0;
}
