#include "compat/include/vm_services.h"
#include "mvdm_vm_bridge.h"

extern VOID ntvdm_clear_instance_data_marking(VOID);
BOOL ntvdm_vm_services_initialize(void)
{
    static const compat_vm_services services = {
        mvdm_vm_bridge_bop, ntvdm_clear_instance_data_marking
    };
    return compat_vm_install(&services);
}
