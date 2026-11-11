#include "ntvdm-exe/session/session.h"
#include <windows.h>

static uint32_t teardown_order[2];
static uint32_t teardown_count;
static session escape_owner;

static DWORD WINAPI foreign_escape(void *context)
{
    session *owner=context;
    if(!session_thread_bind(owner))return 1;
    if(session_arm_termination_escape(owner))return 2;
    session_disarm_termination_escape(owner);
    if(!owner->termination_armed || session_terminate_current(99u))return 3;
    if(owner->state!=SESSION_STATE_ACTIVE || owner->completion_code)return 4;
    return session_thread_unbind(owner) ? 0 : 5;
}

static int check_escape_owner(void)
{
    HANDLE thread;
    DWORD result;
    session_initialize(&escape_owner,303u);
    if(!session_activate(&escape_owner) || !session_thread_bind(&escape_owner) ||
        !session_arm_termination_escape(&escape_owner))return 10;
    if(setjmp(escape_owner.termination_escape)==0) {
        thread=CreateThread(NULL,0,foreign_escape,&escape_owner,0,NULL);
        if(!thread)return 11;
        if(WaitForSingleObject(thread,5000)!=WAIT_OBJECT_0 ||
            !GetExitCodeThread(thread,&result))return 12;
        CloseHandle(thread);
        if(result || !escape_owner.termination_armed)return 13;
        (void)session_terminate_current(37u);
        return 14;
    }
    if(escape_owner.state!=SESSION_STATE_COMPLETED ||
        escape_owner.completion_code!=37u)return 15;
    session_disarm_termination_escape(&escape_owner);
    if(escape_owner.termination_armed || !session_thread_unbind(&escape_owner) ||
        !session_dispose(&escape_owner))return 16;
    /* Disarming must also release the thread-local ownership for reuse. */
    session_initialize(&escape_owner,404u);
    if(!session_activate(&escape_owner) || !session_thread_bind(&escape_owner) ||
        !session_arm_termination_escape(&escape_owner))return 17;
    session_disarm_termination_escape(&escape_owner);
    if(session_terminate_current(99u) || !session_thread_unbind(&escape_owner) ||
        !session_dispose(&escape_owner))return 18;
    return 0;
}

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
    uint8_t *text_writer;
    uint8_t text_copy[8];
    uint32_t text_columns;
    uint32_t text_rows;
    uint32_t text_bytes;

    session_initialize(&first, 101u);
    session_initialize(&second, 202u);
    if (!session_valid(&first) || !session_valid(&second) ||
        session_activate(&first) == 0 || session_activate(&second) == 0)
        return 1;
    if (session_thread_bind(&first) == 0 || session_thread_current() != &first ||
        session_thread_bind(&second) != 0 || session_thread_unbind(&second) != 0 ||
        session_dispose(&first) != 0 || session_thread_unbind(&first) == 0)
        return 2;
    if (session_presentation_text_acquire_writable(&first, 2u, 2u,
            2u, &text_writer) == 0 || text_writer == NULL)
        return 7;
    text_writer[0] = 'N';
    text_writer[1] = 7u;
    if (session_presentation_text_snapshot(&first, text_copy,
            (uint32_t)sizeof(text_copy), &text_columns, &text_rows,
            &text_bytes) == 0 || text_columns != 2u || text_rows != 2u ||
        text_bytes != sizeof(text_copy) || text_copy[0] != 'N' ||
        text_copy[1] != 7u)
        return 8;
    session_presentation_text_clear(&first);
    if (session_presentation_text_snapshot(&first, text_copy,
            (uint32_t)sizeof(text_copy), NULL, NULL, NULL) != 0)
        return 9;
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
    return check_escape_owner();
}
