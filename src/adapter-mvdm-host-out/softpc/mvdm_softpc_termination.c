#include "mvdm_softpc_termination.h"

#include "session/session.h"

int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code)
{
    (void)vdm_for_wow;
    return session_terminate_current(completion_code);
}
