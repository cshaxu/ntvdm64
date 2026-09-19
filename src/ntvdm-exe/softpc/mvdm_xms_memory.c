#include "mvdm_xms_memory.h"

#include "mvdm_softpc_guest_memory.h"
#include "ntvdm-exe/session/session.h"
#include "xms.h"

void mvdm_xms_move_memory(uint32_t destination, uint32_t source, uint32_t byte_count)
{
    session *owner = session_thread_current();

    if (owner == NULL || byte_count > UINT32_MAX - source ||
        byte_count > UINT32_MAX - destination ||
        !mvdm_softpc_guest_memory_move(destination, source, byte_count)) {
        if (owner != NULL) (void)session_request_cancellation(owner,
            SESSION_CANCELLATION_REQUESTED);
    }
}

void mvdm_xms_cancel_current_operation(void)
{
    session *owner = session_thread_current();

    if (owner != NULL) (void)session_request_cancellation(owner,
        SESSION_CANCELLATION_REQUESTED);
}
