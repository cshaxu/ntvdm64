/*
 * CCPU40-only carrier retained beside the original mvdm/dbg/dbg.c body.
 *
 * The original debugger's ThreadSetDebugContext was a kernel VDM service.
 * Its six-DWORD DR0--DR3/DR6/DR7 order is retained here, while descriptor
 * decoding and guest-memory access stay bounded by the sole CCPU40 worker.
 */

#include <stdint.h>

#include "ntvdm-exe/softpc/include/mvdm_softpc_effective_address.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <c_main.h>
#include <c_addr.h>
#include <c_bsic.h>
#include <c_tlb.h>

extern uint16_t c_getSS(void);
extern uint16_t c_getSP(void);
extern void MOV_DR(ULONG index, ULONG value);
extern IBOOL c_getPG(void);
extern IBOOL c_getVM(void);
extern IUH c_getCPL(void);
extern IU32 c_sas_memory_size(void);

static BOOL debug_readable_span(IU32 address, IU32 count, IUM8 access)
{
    IU32 i, physical, size = c_sas_memory_size();

    if (!count || address > UINT32_MAX - (count - 1)) return FALSE;
    for (i = 0; i < count; ++i) {
        physical = address + i;
        if (c_getPG() && !xtrn2phy(address + i, access, &physical)) return FALSE;
        if (physical >= size) return FALSE;
    }
    return TRUE;
}

BOOL mvdm_debugger_read_debug_registers(USHORT selector, USHORT offset,
    PULONG registers)
{
    IU32 descriptor_address, last = (IU32)offset + 23;
    CPU_DESCR descriptor;
    IUM8 access;

    if (registers == NULL || (selector & 0xfffc) == 0 ||
        selector_outside_GDT_LDT(selector, &descriptor_address) ||
        !debug_readable_span(descriptor_address, 8, 0)) return FALSE;
    read_descriptor_linear(descriptor_address, &descriptor);
    if ((descriptor.AR & 0x90) != 0x90 ||
        ((descriptor.AR & 8) && !(descriptor.AR & 2))) return FALSE;
    if ((descriptor.AR & 0x0c) == 4) {
        if (offset <= descriptor.limit ||
            last > ((descriptor.AR & 0x4000) ? UINT32_MAX : 0xffff)) return FALSE;
    } else if (last > descriptor.limit) return FALSE;
    if (descriptor.base > UINT32_MAX - last) return FALSE;
    access = c_getCPL() != 3 ? PG_U : 0;
    if (c_getVM()) access |= 4;
    if (!debug_readable_span(descriptor.base + offset, 24, access)) return FALSE;
    return mvdm_softpc_guest_memory_copy_from(descriptor.base + offset,
        (uint8_t *)registers, 6 * sizeof(*registers));
}

BOOL ThreadSetDebugContext(PULONG registers)
{
    ULONG index;

    if (registers == NULL) return FALSE;
    for (index = 0; index != 4; ++index) MOV_DR(index, registers[index]);
    MOV_DR(6, registers[4]);
    MOV_DR(7, registers[5]);
    return TRUE;
}
