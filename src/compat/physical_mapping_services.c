#include "compat/include/physical_mapping_services.h"

static const compat_physical_mapping_services *binding;

BOOL compat_physical_mapping_install(const compat_physical_mapping_services *services)
{
    const compat_physical_mapping_services *previous;
    if (!services ||
            !services->initialize ||
            !services->release ||
            !services->set_records ||
            !services->map ||
            !services->unmap ||
            !services->translate ||
            !services->resolve ||
            !services->span_is_aliased) {
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

const compat_physical_mapping_services *compat_physical_mapping_current_services(void)
{
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding;
}
