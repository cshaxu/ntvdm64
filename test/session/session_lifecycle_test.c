#include "ntvdm-exe/session/session.h"
#include <windows.h>
#include "compat/include/wow_worker_state.h"

static uint32_t teardown_order[2];
static uint32_t teardown_count;
static int console_closed_in_order;
static session escape_owner;

static DWORD WINAPI foreign_escape(void *context)
{
    session *owner=context;
    if(!session_thread_bind(owner))return 1;
    if(session_arm_termination_escape(owner))return 2;
    session_disarm_termination_escape(owner);
    if(!owner->termination_armed || session_terminate_current(99u))return 3;
    if(owner->host_context.state!=SESSION_STATE_ACTIVE || owner->completion_code ||
        !wow_user_worker_active())return 4;
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
    if(escape_owner.host_context.state!=SESSION_STATE_COMPLETED ||
        escape_owner.completion_code!=37u || wow_user_worker_active())return 15;
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

typedef struct binding_test_hook {
    int fail_bind;
    int fail_unbind;
    unsigned binds;
    unsigned unbinds;
} binding_test_hook;

static int bind_test_hook(void *opaque)
{
    binding_test_hook *hook = opaque;
    ++hook->binds;
    return !hook->fail_bind;
}

static int unbind_test_hook(void *opaque)
{
    binding_test_hook *hook = opaque;
    ++hook->unbinds;
    return !hook->fail_unbind;
}

static int check_binding_rollback(void)
{
    session owner;
    binding_test_hook first = {0};
    binding_test_hook second = {0};
    session_initialize(&owner, 505u);
    if (!session_activate(&owner) ||
        !session_register_thread_hook(&owner, bind_test_hook, unbind_test_hook, &first) ||
        !session_register_thread_hook(&owner, bind_test_hook, unbind_test_hook, &second))
        return 20;
    second.fail_bind = 1;
    if (session_thread_bind(&owner) || session_thread_current() ||
        compat_host_context_current() || owner.binding_count || first.unbinds != 1u)
        return 21;
    first.fail_unbind = 1;
    if (session_thread_bind(&owner) || session_thread_current() != &owner ||
        compat_host_context_current() != &owner.host_context ||
        owner.binding_count != 1 || session_dispose(&owner)) return 22;
    first.fail_unbind = 0;
    if (!session_thread_unbind(&owner) || session_thread_current() ||
        compat_host_context_current() || owner.binding_count) return 23;
    second.fail_bind = 0;
    if (!session_thread_bind(&owner)) return 24;
    second.fail_unbind = 1;
    if (session_thread_unbind(&owner) || session_thread_current() != &owner ||
        owner.binding_count != 1) return 25;
    second.fail_unbind = 0;
    if (!session_thread_unbind(&owner) || !session_dispose(&owner)) return 26;
    return 0;
}

static void record_console_teardown(session *owner)
{
    console_closed_in_order = teardown_count == 2u &&
        owner->host_context.cleanup_count == 0u &&
        owner->host_context.presentation_text_storage != NULL;
    owner->console_client = NULL;
}

static void record_teardown(void *context)
{
    teardown_order[teardown_count++] = *(const uint32_t *)context;
}

static int check_inner_liveness(void)
{
    session owner;
    if (wow_user_worker_active()) return 30;
    session_initialize(&owner, 606u);
    if (!session_activate(&owner) || !session_thread_bind(&owner) ||
            !wow_user_worker_active()) return 31;
    if (!session_request_cancellation(&owner, SESSION_CANCELLATION_REQUESTED) ||
            wow_user_worker_active() ||
            compat_host_context_register(&owner.host_context, record_teardown, NULL))
        return 32;
    if (!session_thread_unbind(&owner) || wow_user_worker_active() ||
            !session_dispose(&owner)) return 33;
    session_initialize(&owner, 707u);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) return 34;
    session_complete(&owner, 42u);
    if (wow_user_worker_active() || owner.completion_code != 42u ||
            !session_thread_unbind(&owner) || !session_dispose(&owner)) return 35;
    return 0;
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
    if (compat_host_text_acquire_writable(&first.host_context, 2u, 2u,
            2u, &text_writer) == 0 || text_writer == NULL)
        return 7;
    text_writer[0] = 'N';
    text_writer[1] = 7u;
    if (compat_host_text_snapshot(&first.host_context, text_copy,
            (uint32_t)sizeof(text_copy), &text_columns, &text_rows,
            &text_bytes) == 0 || text_columns != 2u || text_rows != 2u ||
        text_bytes != sizeof(text_copy) || text_copy[0] != 'N' ||
        text_copy[1] != 7u)
        return 8;
    compat_host_text_clear(&first.host_context);
    if (compat_host_text_snapshot(&first.host_context, text_copy,
            (uint32_t)sizeof(text_copy), NULL, NULL, NULL) != 0)
        return 9;
    if (compat_host_context_register(&first.host_context, record_teardown, (void *)&one) == 0 ||
        compat_host_context_register(&first.host_context, record_teardown, (void *)&two) == 0 ||
        compat_host_context_register(&first.host_context, record_teardown, (void *)&one) == 0 ||
        first.host_context.cleanup_count != 2u)
        return 3;
    session_complete(&second, 37u);
    if (second.host_context.state != SESSION_STATE_COMPLETED || second.completion_code != 37u ||
        session_request_cancellation(&second, SESSION_CANCELLATION_REQUESTED) != 0)
        return 4;
    if (!compat_host_text_acquire_writable(&first.host_context, 1u, 1u, 2u, &text_writer))
        return 6;
    if (session_request_cancellation(&first, SESSION_CANCELLATION_REQUESTED) == 0 ||
        first.host_context.state != SESSION_STATE_CANCELLED ||
        first.cancellation_reason != SESSION_CANCELLATION_REQUESTED ||
        compat_host_context_register(&first.host_context, record_teardown, (void *)&one) != 0)
        return 5;
    first.console_client = &first;
    if (session_dispose(&first) || teardown_count != 0u ||
        !session_dispose_with_reason(&first, NULL, record_console_teardown) ||
        !console_closed_in_order || session_valid(&first) ||
        teardown_count != 2u || teardown_order[0] != 2u || teardown_order[1] != 1u ||
        session_dispose(&second) == 0)
        return 6;
    {
        int result = check_binding_rollback();
        if (result) return result;
        result = check_escape_owner();
        return result ? result : check_inner_liveness();
    }
}
