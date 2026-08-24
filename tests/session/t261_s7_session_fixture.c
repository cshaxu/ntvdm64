#include "session/ntdos64_session_v1.h"
#include "adapter-softpc/bx_ntvdm_guest_pointer_manager.h"

static unsigned int teardown_calls;

static void teardown(void)
{
    ++teardown_calls;
}

int main(void)
{
    ntdos64_session_v1 session;
    ntdos64_session_v1 other;

    ntdos64_session_v1_initialize(&session);
    if (!ntdos64_session_v1_valid(&session) ||
        ntdos64_session_v1_activate(&session) == 0 ||
        ntdos64_session_v1_register_teardown(&session, teardown) == 0 ||
        ntdos64_session_v1_register_teardown(&session, teardown) == 0 ||
        bx_ntvdm_session_mapping_registry_bind(&session) == 0 ||
        ntdos64_session_v1_register_teardown(&session,
            bx_ntvdm_session_mapping_registry_reset) == 0 ||
        session.teardown_count != 2u) return 1;
    ntdos64_session_v1_initialize(&other);
    if (ntdos64_session_v1_activate(&other) == 0 ||
        bx_ntvdm_session_mapping_registry_bind(&other) != 0) return 4;
    ntdos64_session_v1_complete(&session, 37u);
    if (session.state != NTDOS64_SESSION_V1_COMPLETED ||
        session.completion_code != 37u) return 2;
    ntdos64_session_v1_reset(&session);
    if (teardown_calls != 1u || !ntdos64_session_v1_valid(&session) ||
        session.state != NTDOS64_SESSION_V1_INACTIVE ||
        session.epoch != 0u) return 3;
    return 0;
}
