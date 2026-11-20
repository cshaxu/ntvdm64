#include "compat/include/vm_services.h"
#include "compat/include/mvdm_softpc_termination.h"

static const compat_vm_services *binding;

BOOL compat_vm_install(const compat_vm_services *services)
{
    const compat_vm_services *previous;
    if (!services || !services->bop) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&binding,
        (PVOID)services, NULL);
    if (previous && previous != services) {
        SetLastError(ERROR_ALREADY_INITIALIZED);
        return FALSE;
    }
    return TRUE;
}

const compat_vm_services *compat_vm_current_services(void)
{
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding;
}

/* DIVERGENCE(ADAPTER-SOFTPC-034): this profile has no VDD instance-data
 * provider. Leave through the existing controlled stop instead of reporting
 * success to a guest that requires the original cleanup. */
VOID ClearInstanceDataMarking(VOID)
{
    (void)mvdm_softpc_terminate_current_session(0u, ERROR_CALL_NOT_IMPLEMENTED);
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}
