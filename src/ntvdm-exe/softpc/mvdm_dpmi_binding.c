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

extern ULONG __cdecl c_sas_dw_at(ULONG address);
extern int __cdecl c_sas_twenty_bit_wrapping_enabled(void);
extern void __cdecl c_sas_fills(ULONG address, BYTE value, ULONG count);
extern void __cdecl c_sas_storedw(ULONG address, ULONG value);
extern void __cdecl c_sas_storew(ULONG address, USHORT value);
extern void __cdecl c_sas_loads(ULONG address, BYTE *bytes, ULONG count);
extern void __cdecl c_sas_stores(ULONG address, BYTE *bytes, ULONG count);
extern BYTE * __cdecl c_GetLinAdd(ULONG address);
extern BYTE * __cdecl c_GetPhyAdd(ULONG address);
extern ULONG __cdecl c_getCR0(void);
extern ULONG __cdecl c_getCR3(void);
extern void __cdecl c_setCR0(ULONG value);
extern void __cdecl c_setCR3(ULONG value);
extern ULONG __cdecl c_getGDT_BASE(void);
extern USHORT __cdecl c_getGDT_LIMIT(void);
extern void __cdecl c_setGDT_BASE_LIMIT(ULONG base, USHORT limit);
extern LONG __cdecl VdmAllocateVirtualMemory(ULONG *address, ULONG bytes, BOOL commit);
extern LONG __cdecl VdmFreeVirtualMemory(ULONG address);
extern LONG __cdecl VdmAddVirtualMemory(ULONG host_address, ULONG bytes, ULONG *intel_address);
extern LONG __cdecl VdmRemoveVirtualMemory(ULONG address);

extern ULONG __cdecl c_getEAX(void);
extern ULONG __cdecl c_getECX(void);
extern ULONG __cdecl c_getEDX(void);
extern ULONG __cdecl c_getESI(void);
extern ULONG __cdecl c_getEBX(void);
extern int __cdecl c_getPE(void);
extern void __cdecl c_setEAX(ULONG value);
extern void __cdecl c_setEBX(ULONG value);
extern void __cdecl c_setEDX(ULONG value);
extern void __cdecl c_setESI(ULONG value);
extern void __cdecl c_setCF(int value);
extern void __cdecl virtual_device_trap(void);

extern void __cdecl host_ica_lock(void);
extern void __cdecl host_ica_unlock(void);
extern WORD __cdecl VDDReserveIrqLine(HANDLE, WORD);
extern BOOL __cdecl VDDReleaseIrqLine(HANDLE, WORD);
extern BOOL __cdecl RegisterEOIHook(int, void (*)(int, int));
extern BOOL __cdecl RemoveEOIHook(int, void (*)(int, int));
extern void __cdecl ica_hw_interrupt(ULONG, ULONG, LONG);
extern BOOL __cdecl BiosKeyToInputRecord(PKEY_EVENT_RECORD);
extern USHORT __cdecl c_getIP(void);
extern void __cdecl c_setIP(USHORT);
extern void __cdecl CALLF(unsigned long [2]);

BOOL ntvdm_dpmi_initialize(void)
{
    static const mvdm_cpu_access cpu = {
        ccpu_capture_debug_context, ccpu_restore_debug_context,
        ccpu_apply_debug_context, c_sas_memory_size, c_getPG, c_getVM,
        c_getCPL, selector_outside_GDT_LDT, ccpu_read_descriptor_fields, xtrn2phy,
        mvdm_softpc_guest_memory_copy_from, c_getMSW, c_getEFLAGS,
        host_DelayHwInterrupt, ica_hw_interrupt_cancel,
        mvdm_guest_location_acquire, mvdm_guest_location_release,
        mvdm_guest_location_copy_to_guest,
        c_sas_dw_at, c_sas_twenty_bit_wrapping_enabled, c_sas_fills,
        c_sas_storedw, c_sas_storew, c_sas_loads,
        c_sas_stores, c_GetLinAdd, c_GetPhyAdd,
        c_getCR0, c_getCR3, c_setCR0,
        c_setCR3, c_getGDT_BASE, c_getGDT_LIMIT,
        c_setGDT_BASE_LIMIT, VdmAllocateVirtualMemory, VdmFreeVirtualMemory,
        VdmAddVirtualMemory, VdmRemoveVirtualMemory,
        c_getEAX, c_getECX, c_getEDX,
        c_getESI, c_getEBX, c_getPE,
        c_setEAX, c_setEBX, c_setEDX,
        c_setESI, c_setCF, virtual_device_trap,
        host_ica_lock, host_ica_unlock, VDDReserveIrqLine, VDDReleaseIrqLine, RegisterEOIHook, RemoveEOIHook, ica_hw_interrupt, BiosKeyToInputRecord, c_getIP, c_setIP, CALLF, mvdm_softpc_guest_memory_acquire, mvdm_softpc_guest_memory_release, mvdm_softpc_guest_memory_copy_from, mvdm_softpc_guest_memory_copy_to
    };
    static const compat_dpmi_services services = {
        ThreadSetDebugContext, mvdm_debugger_read_debug_registers,
        mvdm_softpc_wow_page_domain_leave_protected,
        mvdm_softpc_wow_page_domain_reenter_protected
    };
    return mvdm_cpu_access_install(&cpu) && compat_dpmi_install(&services);
}
