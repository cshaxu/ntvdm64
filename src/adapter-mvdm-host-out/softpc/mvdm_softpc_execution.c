#include "mvdm_softpc_execution.h"

/* Original SoftPC host spelling; its body remains in mvdm-host. */
extern void host_start_cpu(void);

int mvdm_softpc_execution_run_until_return(session *owner)
{
    int did_bind = 0;

    if (owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE ||
        session_machine_backend(owner) != SESSION_MACHINE_BACKEND_SOFTPC)
        return 0;

    if (session_thread_current() != owner) {
        if (!session_thread_bind(owner)) return 0;
        did_bind = 1;
    }

    /* The original host entry owns CPU execution and may return only at its
     * original CCPU boundary.  This bridge owns no CPU, BOP or BIOS meaning. */
    host_start_cpu();
    session_record_mechanical_resume_status(owner,
        SESSION_MECHANICAL_STATUS_SOFTPC_RETURNED);

    if (did_bind) (void)session_thread_unbind(owner);
    return 1;
}
