#include "include/mvdm_softpc_termination.h"

#include "include/host_context.h"
#include <windows.h>

/* Original host close is idempotent: it rejects both pre-start and repeated
 * calls. Direct standalone termination otherwise longjmps around the normal
 * original host_main -> host_applClose tail. */
static void (*close_original_host)(void);

int mvdm_softpc_termination_bind(void (*close_host)(void))
{
    void *previous;
    if (!close_host) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    previous=InterlockedCompareExchangePointer((PVOID volatile *)&close_original_host,
        (PVOID)close_host,NULL);
    if (previous && previous != (PVOID)close_host) {
        SetLastError(ERROR_ALREADY_INITIALIZED); return 0;
    }
    return 1;
}

int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code)
{
    (void)vdm_for_wow;
    /* Restore the original host close cohort before the standalone escape.
     * In NT4 this escape was ExitProcess; in this process it is longjmp, so
     * an outer application entry cannot be the cleanup owner. */
    if (!close_original_host) RaiseFailFastException(NULL,NULL,0);
    close_original_host();
    return compat_host_escape_terminate_current(completion_code);
}

/* ADAPTER-SOFTPC-034: this profile has no VDD instance-data provider. */
void ClearInstanceDataMarking(void)
{
    (void)mvdm_softpc_terminate_current_session(0u, ERROR_CALL_NOT_IMPLEMENTED);
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}
