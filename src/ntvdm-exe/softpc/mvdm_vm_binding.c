#include "compat/include/vm_services.h"
#include "mvdm_vm_bridge.h"

BOOL ntvdm_vm_services_initialize(void)
{
    static const compat_vm_services services = {
        mvdm_vm_bridge_bop
    };
    return compat_vm_install(&services);
}
