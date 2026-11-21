#include "include/mvdm_cpu_access.h"

static const mvdm_cpu_access *binding;

BOOL mvdm_cpu_access_install(const mvdm_cpu_access *provider)
{
    const mvdm_cpu_access *previous;
    if (!provider || !provider->capture_debug || !provider->restore_debug ||
        !provider->apply_debug || !provider->memory_size || !provider->paging ||
        !provider->virtual_mode || !provider->privilege ||
        !provider->selector_outside || !provider->descriptor_fields ||
        !provider->translate || !provider->read_memory ||
        !provider->machine_status || !provider->flags ||
        !provider->delay_interrupt || !provider->cancel_interrupt ||
        !provider->acquire_location || !provider->release_location ||
        !provider->copy_to_guest) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&binding,
        (PVOID)provider, NULL);
    if (previous && previous != provider) {
        SetLastError(ERROR_ALREADY_INITIALIZED);
        return FALSE;
    }
    return TRUE;
}

const mvdm_cpu_access *mvdm_cpu_access_current(void)
{
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding;
}
