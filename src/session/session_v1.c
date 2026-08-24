#include "session_v1.h"

#include <string.h>

static uint32_t next_identity;

void ntdos64_session_v1_initialize(ntdos64_session_v1 *session)
{
    if (session == 0) return;
    memset(session, 0, sizeof(*session));
    session->magic = NTDOS64_SESSION_V1_MAGIC;
    session->abi_version = NTDOS64_SESSION_V1_VERSION;
    session->struct_bytes = (uint32_t)sizeof(*session);
    if (++next_identity == 0u) ++next_identity;
    session->identity = next_identity;
}

int ntdos64_session_v1_valid(const ntdos64_session_v1 *session)
{
    return session != 0 && session->magic == NTDOS64_SESSION_V1_MAGIC &&
        session->abi_version == NTDOS64_SESSION_V1_VERSION &&
        session->struct_bytes == sizeof(*session) &&
        session->identity != 0u &&
        session->state <= NTDOS64_SESSION_V1_COMPLETED &&
        session->teardown_count <= NTDOS64_SESSION_V1_MAX_TEARDOWNS &&
        session->reserved0 == 0u;
}

int ntdos64_session_v1_activate(ntdos64_session_v1 *session)
{
    if (!ntdos64_session_v1_valid(session) ||
        session->state != NTDOS64_SESSION_V1_INACTIVE) return 0;
    if (++session->epoch == 0u) session->epoch = 1u;
    session->state = NTDOS64_SESSION_V1_ACTIVE;
    return 1;
}

int ntdos64_session_v1_register_teardown(ntdos64_session_v1 *session,
    ntdos64_session_v1_teardown_fn teardown)
{
    uint32_t index;
    if (!ntdos64_session_v1_valid(session) ||
        session->state != NTDOS64_SESSION_V1_ACTIVE || teardown == 0) return 0;
    for (index = 0u; index < session->teardown_count; ++index)
        if (session->teardowns[index] == teardown) return 1;
    if (session->teardown_count == NTDOS64_SESSION_V1_MAX_TEARDOWNS) return 0;
    session->teardowns[session->teardown_count++] = teardown;
    return 1;
}

void ntdos64_session_v1_complete(ntdos64_session_v1 *session,
    uint32_t completion_code)
{
    if (!ntdos64_session_v1_valid(session) ||
        session->state != NTDOS64_SESSION_V1_ACTIVE) return;
    session->completion_code = completion_code;
    session->state = NTDOS64_SESSION_V1_COMPLETED;
}

void ntdos64_session_v1_reset(ntdos64_session_v1 *session)
{
    uint32_t index;
    if (!ntdos64_session_v1_valid(session)) return;
    for (index = session->teardown_count; index != 0u; --index)
        session->teardowns[index - 1u]();
    ntdos64_session_v1_initialize(session);
}
