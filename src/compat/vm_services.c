#include "compat/include/vm_services.h"

static const compat_vm_services *binding;

BOOL compat_vm_install(const compat_vm_services *services)
{
    const compat_vm_services *previous;
    if (!services || !services->bop || !services->clear_instance_data) {
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

VOID ClearInstanceDataMarking(VOID)
{
    compat_vm_current_services()->clear_instance_data();
}
