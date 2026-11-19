#ifndef COMPAT_DPMI_HOST_H
#define COMPAT_DPMI_HOST_H
#include "dpmi_services.h"

/* Preserve the existing caller signatures and sequencing. */
#define ThreadSetDebugContext (compat_dpmi_current_services()->set_debug_context)
#define mvdm_debugger_read_debug_registers (compat_dpmi_current_services()->read_debug_registers)
#define mvdm_softpc_wow_page_domain_leave_protected (compat_dpmi_current_services()->leave_protected)
#define mvdm_softpc_wow_page_domain_reenter_protected (compat_dpmi_current_services()->reenter_protected)
#endif
