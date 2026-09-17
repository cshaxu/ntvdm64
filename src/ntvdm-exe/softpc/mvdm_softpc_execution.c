#include "mvdm_softpc_execution.h"
#include "mvdm_softpc_guest_memory.h"
#include "mvdm_softpc_termination.h"

/* Original SoftPC host spelling; its body remains in mvdm-host. */
extern void host_start_cpu(void);
extern void host_applClose(void);
/* The bounded execution fixture starts directly at host_start_cpu(), unlike
 * the original ntvdm.c -> host_main path.  It therefore retains this original
 * close cohort after that fixture-only entry returns. */
static void mvdm_softpc_execution_close_original_host(void)
{
    host_applClose();
}
int mvdm_softpc_execution_run_until_return(session *owner)
{
    int did_bind = 0;
    int did_memory_bind = 0;

    if (owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE ||
        session_machine_backend(owner) != SESSION_MACHINE_BACKEND_SOFTPC)
        return 0;

    if (session_thread_current() != owner) {
        if (!session_thread_bind_owned(owner,
                SESSION_THREAD_BINDING_SOFTPC_ENTRY)) return 0;
        did_bind = 1;
    }

    if (!mvdm_softpc_guest_memory_begin(owner)) {
        if (did_bind) (void)session_thread_unbind(owner);
        return 0;
    }
    did_memory_bind = 1;

    if (!session_arm_termination_escape(owner)) {
        if (did_memory_bind) mvdm_softpc_guest_memory_end(owner);
        if (did_bind) (void)session_thread_unbind(owner);
        return 0;
    }
    if (setjmp(owner->termination_escape) != 0) {
        mvdm_softpc_execution_close_original_host();
        session_disarm_termination_escape(owner);
        if (did_memory_bind) mvdm_softpc_guest_memory_end(owner);
        if (did_bind) (void)session_thread_unbind(owner);
        return 1;
    }

    /* The original host entry owns CPU execution and may return only at its
     * original CCPU boundary.  This bridge owns no CPU, BOP or BIOS meaning. */
    host_start_cpu();
    mvdm_softpc_execution_close_original_host();
    session_disarm_termination_escape(owner);
    if (did_memory_bind) mvdm_softpc_guest_memory_end(owner);

    if (did_bind) (void)session_thread_unbind(owner);
    return 1;
} /* bounded host_start_cpu fixture bridge */
