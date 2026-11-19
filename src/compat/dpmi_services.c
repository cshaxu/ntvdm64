#include "compat/include/dpmi_services.h"

static const compat_dpmi_services *binding;

BOOL compat_dpmi_install(const compat_dpmi_services *services)
{
    const compat_dpmi_services *previous;
    if (!services || !services->set_debug_context || !services->read_debug_registers ||
            !services->leave_protected || !services->reenter_protected) {
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

const compat_dpmi_services *compat_dpmi_current_services(void)
{
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding;
}
