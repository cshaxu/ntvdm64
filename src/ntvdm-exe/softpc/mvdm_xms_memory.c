#include "mvdm_xms_memory.h"

#include "ntvdm-exe/session/session.h"

void mvdm_xms_cancel_current_operation(void)
{
    session *owner = session_thread_current();

    if (owner != NULL) (void)session_request_cancellation(owner,
        SESSION_CANCELLATION_REQUESTED);
}
