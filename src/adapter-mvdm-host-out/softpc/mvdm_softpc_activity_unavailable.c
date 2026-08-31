/*
 * The selected OpenNT trees retain callers of the generated CCPU activity
 * callback but no provider body.  Replacing it with a no-op, yield, or timer
 * would falsely claim CPU scheduling semantics.  An actual call therefore
 * ends only the active session with an explicit unavailable result.
 */

#include <windows.h>

#include "session/session.h"

/* DIVERGENCE(ADAPTER-SOFTPC-033): preserve the original generated CPU40
 * callback spelling.  Its historical provider is absent from the selected
 * source package; an invoked callback is a typed session stop, not a fake
 * scheduler tick or a process termination. */
VOID ActivityCheckAfterTimeSlice(VOID)
{
    session *owner = session_thread_current();

    if (owner != NULL) {
        session_record_mechanical_resume_status(owner,
            SESSION_MECHANICAL_STATUS_BACKEND_UNAVAILABLE);
        (void)session_terminate_current((uint32_t)ERROR_CALL_NOT_IMPLEMENTED);
    }
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}
