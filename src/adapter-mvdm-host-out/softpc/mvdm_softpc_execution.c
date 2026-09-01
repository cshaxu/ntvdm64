#include "mvdm_softpc_execution.h"
#include "mvdm_softpc_guest_memory.h"

/* Original SoftPC host spelling; its body remains in mvdm-host. */
extern void host_start_cpu(void);
extern void host_applClose(void);
/* Original `softpc.new/obj.vdm/ntvdm.c` body; the formal graph renames only
 * its C entry symbol to leave application composition as the product entry. */
extern int mvdm_softpc_original_entry(int argc, char **argv);

/* The original `host_main` normally reaches this cohort itself.  The app
 * owns a bounded in-process session rather than NT4's dedicated process, so
 * every return to the composition boundary also asks the original close body
 * to finish its once-per-host-start cleanup.  `host_applClose` retains the
 * source-owned order and rejects a pre-start or repeated close. */
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
        session_record_mechanical_resume_status(owner,
            SESSION_MECHANICAL_STATUS_SOFTPC_RETURNED);
        mvdm_softpc_execution_close_original_host();
        session_disarm_termination_escape(owner);
        if (did_memory_bind) mvdm_softpc_guest_memory_end(owner);
        if (did_bind) (void)session_thread_unbind(owner);
        return 1;
    }

    /* The original host entry owns CPU execution and may return only at its
     * original CCPU boundary.  This bridge owns no CPU, BOP or BIOS meaning. */
    host_start_cpu();
    session_record_mechanical_resume_status(owner,
        SESSION_MECHANICAL_STATUS_SOFTPC_RETURNED);
    mvdm_softpc_execution_close_original_host();
    session_disarm_termination_escape(owner);
    if (did_memory_bind) mvdm_softpc_guest_memory_end(owner);

    if (did_bind) (void)session_thread_unbind(owner);
    return 1;
}

int mvdm_softpc_execution_run_original_entry(session *owner, int argc,
    char **argv, int *result_out)
{
    int did_bind = 0;
    int did_memory_bind = 0;
    int result;

    if (result_out == NULL || owner == NULL || !session_valid(owner) ||
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
        *result_out = (int)owner->completion_code;
        /* A source-shaped unavailable boundary may have already recorded the
         * reason for this non-local return.  Do not relabel that controlled
         * stop as an ordinary SoftPC return. */
        if (session_mechanical_resume_status(owner) ==
            SESSION_MECHANICAL_STATUS_NONE) {
            session_record_mechanical_resume_status(owner,
                SESSION_MECHANICAL_STATUS_SOFTPC_RETURNED);
        }
        mvdm_softpc_execution_close_original_host();
        session_disarm_termination_escape(owner);
        if (did_memory_bind) mvdm_softpc_guest_memory_end(owner);
        if (did_bind) (void)session_thread_unbind(owner);
        return 1;
    }

    /* Do not split or reproduce the historical entry sequence here.  The
     * original `ntvdm.c` entry owns timer, environment, NLS, host_main and
     * eventual source-shaped return ordering. */
    mvdm_softpc_set_termination_origin("original-entry-unattributed");
    result = mvdm_softpc_original_entry(argc, argv);
    *result_out = result;
    session_record_mechanical_resume_status(owner,
        SESSION_MECHANICAL_STATUS_SOFTPC_RETURNED);
    mvdm_softpc_execution_close_original_host();
    session_disarm_termination_escape(owner);
    if (did_memory_bind) mvdm_softpc_guest_memory_end(owner);

    if (did_bind) (void)session_thread_unbind(owner);
    return 1;
}
