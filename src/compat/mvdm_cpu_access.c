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
        !provider->copy_to_guest ||
        !provider->read_dword ||
        !provider->twenty_bit_wrapping ||
        !provider->fill_bytes ||
        !provider->store_dword ||
        !provider->store_word ||
        !provider->load_bytes ||
        !provider->store_bytes ||
        !provider->linear_pointer ||
        !provider->physical_pointer ||
        !provider->cr0 ||
        !provider->cr3 ||
        !provider->set_cr0 ||
        !provider->set_cr3 ||
        !provider->gdt_base ||
        !provider->gdt_limit ||
        !provider->set_gdt ||
        !provider->allocate_virtual_memory ||
        !provider->free_virtual_memory ||
        !provider->add_virtual_memory ||
        !provider->remove_virtual_memory ||
        !provider->eax ||
        !provider->ecx ||
        !provider->edx ||
        !provider->esi ||
        !provider->ebx ||
        !provider->protected_mode ||
        !provider->set_eax ||
        !provider->set_ebx ||
        !provider->set_edx ||
        !provider->set_esi ||
        !provider->set_carry ||
        !provider->unhandled_bop) {
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
