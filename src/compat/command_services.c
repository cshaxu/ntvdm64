#include "compat/include/command_services.h"

static const compat_command_services *binding;

BOOL compat_command_install(const compat_command_services *services)
{
    const compat_command_services *previous;
    if (!services || !services->capture_guest || !services->capture_host_command ||
            !services->std_handles || !services->command || !services->environment ||
            !services->replace_command || !services->select_service || !services->finish ||
            !services->set_std_handle || !services->create_process) {
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

const compat_command_services *compat_command_current_services(void)
{
    /* Missing product composition is fatal, never an implicit native launch. */
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding;
}
