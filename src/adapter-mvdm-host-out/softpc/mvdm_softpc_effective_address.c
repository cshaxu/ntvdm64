#include "mvdm_softpc_effective_address.h"

/* The CCPU translation units own selector-cache and descriptor semantics.
 * This adapter retains the historical external `c_effective_addr` name while
 * asking those original routines for a numeric guest-linear result. */
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <c_main.h>
#include <c_addr.h>
#include <c_bsic.h>
#include <c_prot.h>
#include <c_seg.h>
#include <c_stack.h>
#include <c_xcptn.h>
#include <c_reg.h>
#include <c_page.h>

/* cpu4gen.h publishes these declarations to CCPU clients, but including the
 * generated vector header here would also import its unrelated generated ABI.
 * Keep this adapter's private declarations exactly in the original IU forms. */
IMPORT IU16 c_getMSW IPT0();
IMPORT IU16 c_getCS_SELECTOR IPT0();
IMPORT IU16 c_getSS_SELECTOR IPT0();
IMPORT IU16 c_getDS_SELECTOR IPT0();
IMPORT IU16 c_getES_SELECTOR IPT0();
IMPORT IU16 c_getFS_SELECTOR IPT0();
IMPORT IU16 c_getGS_SELECTOR IPT0();
IMPORT IU32 c_getCS_BASE IPT0();
IMPORT IU32 c_getSS_BASE IPT0();
IMPORT IU32 c_getDS_BASE IPT0();
IMPORT IU32 c_getES_BASE IPT0();
IMPORT IU32 c_getFS_BASE IPT0();
IMPORT IU32 c_getGS_BASE IPT0();

#define CCPU_MSW_PE      ((IU16)0x0001)
#define CCPU_EFLAGS_VM   ((IU32)0x00020000)

static int current_segment_base(IU16 selector, IU32 *base_out)
{
    if (base_out == 0) return 0;
    if (selector == c_getCS_SELECTOR()) { *base_out = c_getCS_BASE(); return 1; }
    if (selector == c_getSS_SELECTOR()) { *base_out = c_getSS_BASE(); return 1; }
    if (selector == c_getDS_SELECTOR()) { *base_out = c_getDS_BASE(); return 1; }
    if (selector == c_getES_SELECTOR()) { *base_out = c_getES_BASE(); return 1; }
    if (selector == c_getFS_SELECTOR()) { *base_out = c_getFS_BASE(); return 1; }
    if (selector == c_getGS_SELECTOR()) { *base_out = c_getGS_BASE(); return 1; }
    return 0;
}

IU32 c_effective_addr(IU16 selector, IU32 offset)
{
    IU32 base;
    IU32 descriptor_address;
    CPU_DESCR descriptor;

    if ((c_getMSW() & CCPU_MSW_PE) == 0 ||
        (c_getEFLAGS() & CCPU_EFLAGS_VM) != 0) {
        return ((IU32)selector << 4) + offset;
    }
    if (!current_segment_base(selector, &base)) {
        /* Preserve the original CCPU descriptor-table algorithm rather than
         * reintroducing Sim32GetVDMPointer or a monitor pointer alias. */
        if (selector_outside_table(selector, &descriptor_address) != 0) return 0u;
        read_descriptor_linear(descriptor_address, &descriptor);
        base = descriptor.base;
    }
    return base + offset;
}
