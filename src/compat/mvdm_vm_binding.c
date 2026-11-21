#include "include/mvdm_vm_bridge.h"

#include "include/mvdm_cpu_access.h"

static void vm_device_bop(void)
{
    const mvdm_cpu_access *cpu = mvdm_cpu_access_current();
    mvdm_vm_bridge_result result;
    if (!mvdm_vm_bridge_bop(cpu->eax(), cpu->ecx(), cpu->edx(), cpu->esi(), cpu->ebx(),
            cpu->protected_mode() && !cpu->virtual_mode() && cpu->privilege() == 0, &result)) {
        cpu->unhandled_bop();
        return;
    }
    cpu->set_eax(result.status);
    cpu->set_ebx(result.value);
    cpu->set_edx(result.serial_low);
    cpu->set_esi(result.serial_high);
    cpu->set_carry(result.status != 0);
}

BOOL ntvdm_vm_services_initialize(void (**slot)(void))
{
    const mvdm_cpu_access *cpu = mvdm_cpu_access_current();
    if (!slot) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (*slot == vm_device_bop) return TRUE;
    if (*slot != cpu->unhandled_bop) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    *slot = vm_device_bop;
    return TRUE;
}
