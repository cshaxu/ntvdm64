#include "compat/include/dpmi_services.h"
#include "compat/include/mvdm_cpu_access.h"
#include "compat/include/guest_memory_access.h"
#include "mvdm_softpc_wow_page_domain.h"

extern BOOL ThreadSetDebugContext(PULONG registers);
extern BOOL mvdm_debugger_read_debug_registers(USHORT selector, USHORT offset,
    PULONG registers);

extern void __cdecl ccpu_capture_debug_context(ULONG *, BOOL *);
extern void __cdecl ccpu_restore_debug_context(const ULONG *, BOOL);
extern void __cdecl ccpu_apply_debug_context(const ULONG *);
extern ULONG __cdecl c_sas_memory_size(void);
extern int __cdecl c_getPG(void), c_getVM(void);
extern unsigned int __cdecl c_getCPL(void);
extern BOOL __cdecl selector_outside_GDT_LDT(USHORT, ULONG *);
extern void __cdecl ccpu_read_descriptor_fields(ULONG, ULONG *, ULONG *, USHORT *);
extern int __cdecl xtrn2phy(ULONG, unsigned char, ULONG *);
extern USHORT __cdecl c_getMSW(void);
extern ULONG __cdecl c_getEFLAGS(void);
extern BOOL __cdecl host_DelayHwInterrupt(int, int, ULONG);
extern void __cdecl ica_hw_interrupt_cancel(ULONG, ULONG);

BOOL ntvdm_dpmi_initialize(void)
{
    static const mvdm_cpu_access cpu = {
        ccpu_capture_debug_context, ccpu_restore_debug_context,
        ccpu_apply_debug_context, c_sas_memory_size, c_getPG, c_getVM,
        c_getCPL, selector_outside_GDT_LDT, ccpu_read_descriptor_fields, xtrn2phy,
        mvdm_softpc_guest_memory_copy_from, c_getMSW, c_getEFLAGS,
        host_DelayHwInterrupt, ica_hw_interrupt_cancel
    };
    static const compat_dpmi_services services = {
        ThreadSetDebugContext, mvdm_debugger_read_debug_registers,
        mvdm_softpc_wow_page_domain_leave_protected,
        mvdm_softpc_wow_page_domain_reenter_protected
    };
    return mvdm_cpu_access_install(&cpu) && compat_dpmi_install(&services);
}
