/*
 * The selected ordinary profile has no VDD instance-data product.  Its only
 * reached original call is the VxD system-exit cleanup hook.  It must not be
 * converted into a successful no-op because that would hide stale instance
 * state from a VDD-aware guest path.
 */

#include <windows.h>

#include "session/session.h"

/* DIVERGENCE(ADAPTER-SOFTPC-034): retain the original VDD cleanup entrypoint
 * name.  When the unsupported VDD path is actually entered, mark the bound
 * session unavailable and return through its typed controlled-stop boundary. */
VOID ClearInstanceDataMarking(VOID)
{
    session *owner = session_thread_current();

    if (owner != NULL) {
        session_record_mechanical_resume_status(owner,
            SESSION_MECHANICAL_STATUS_BACKEND_UNAVAILABLE);
        (void)session_terminate_current((uint32_t)ERROR_CALL_NOT_IMPLEMENTED);
    }
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}
