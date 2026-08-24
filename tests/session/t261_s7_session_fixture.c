#include "session/session.h"
#include "adapter-softpc/guest_pointer_manager.h"

static unsigned int teardown_calls;

static void teardown(void)
{
    ++teardown_calls;
}

int main(void)
{
    app_session_v1 session;
    app_session_v1 other;

    app_session_v1_initialize(&session);
    if (!app_session_v1_valid(&session) ||
        app_session_v1_activate(&session) == 0 ||
        app_session_v1_register_teardown(&session, teardown) == 0 ||
        app_session_v1_register_teardown(&session, teardown) == 0 ||
        runtime_session_mapping_registry_bind(&session) == 0 ||
        app_session_v1_register_teardown(&session,
            runtime_session_mapping_registry_reset) == 0 ||
        session.teardown_count != 2u) return 1;
    app_session_v1_initialize(&other);
    if (app_session_v1_activate(&other) == 0 ||
        runtime_session_mapping_registry_bind(&other) != 0) return 4;
    app_session_v1_complete(&session, 37u);
    if (session.state != APP_SESSION_V1_COMPLETED ||
        session.completion_code != 37u) return 2;
    app_session_v1_reset(&session);
    if (teardown_calls != 1u || !app_session_v1_valid(&session) ||
        session.state != APP_SESSION_V1_INACTIVE ||
        session.epoch != 0u) return 3;
    return 0;
}
