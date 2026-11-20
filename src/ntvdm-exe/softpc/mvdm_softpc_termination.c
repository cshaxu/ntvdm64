#include "mvdm_softpc_termination.h"

#include "ntvdm-exe/session/session.h"

/* Original host close is idempotent: it rejects both pre-start and repeated
 * calls. Direct standalone termination otherwise longjmps around the normal
 * original host_main -> host_applClose tail. */
extern void host_applClose(void);

int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code)
{
    (void)vdm_for_wow;
    /* Restore the original host close cohort before the standalone escape.
     * In NT4 this escape was ExitProcess; in this process it is longjmp, so
     * an outer application entry cannot be the cleanup owner. */
    host_applClose();
    return session_terminate_current(completion_code);
}
