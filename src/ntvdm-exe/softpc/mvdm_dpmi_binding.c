#include "compat/include/dpmi_services.h"
#include "mvdm_softpc_wow_page_domain.h"

extern BOOL ThreadSetDebugContext(PULONG registers);
extern BOOL mvdm_debugger_read_debug_registers(USHORT selector, USHORT offset,
    PULONG registers);

BOOL ntvdm_dpmi_initialize(void)
{
    static const compat_dpmi_services services = {
        ThreadSetDebugContext, mvdm_debugger_read_debug_registers,
        mvdm_softpc_wow_page_domain_leave_protected,
        mvdm_softpc_wow_page_domain_reenter_protected
    };
    return compat_dpmi_install(&services);
}
