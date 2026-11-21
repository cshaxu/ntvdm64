#include "include/mvdm_softpc_effective_address.h"

/* The CCPU translation units own selector-cache and descriptor semantics.
 * This adapter retains the historical external `c_effective_addr` name while
 * asking those original routines for a numeric guest-linear result. */
#include "include/mvdm_cpu_access.h"

#define CCPU_MSW_PE      ((USHORT)0x0001)
#define CCPU_EFLAGS_VM   ((ULONG)0x00020000)

/* CPU40 keeps descriptor state inside the original CCPU core.  Its original
 * `selector_outside_GDT_LDT` and `read_descriptor_linear` routines are the
 * same CCPU40 mechanism used by protected-mode instruction paths.  Do not
 * recreate a CPU30 monitor descriptor table in this adapter. */
int __cdecl mvdm_softpc_protected_segment(USHORT selector, ULONG *base,
                                          ULONG *limit, ULONG *access)
{
    ULONG descriptor_address;
    USHORT descriptor_access;

    if (base == 0 || limit == 0 || access == 0 ||
        mvdm_cpu_access_current()->selector_outside(selector, &descriptor_address)) return 0;
    mvdm_cpu_access_current()->descriptor_fields(descriptor_address, base, limit,
        &descriptor_access);
    *access = descriptor_access;
    return 1;
}

int mvdm_softpc_protected_address(USHORT selector, ULONG offset, ULONG *address_out)
{
    ULONG base, limit, access;
    if (address_out == 0 ||
        !mvdm_softpc_protected_segment(selector, &base, &limit, &access)) return 0;
    *address_out = base + offset;
    return 1;
}

int mvdm_softpc_effective_address(USHORT selector, ULONG offset, ULONG *address_out)
{
    if (address_out == 0) return 0;
    if ((mvdm_cpu_access_current()->machine_status() & CCPU_MSW_PE) == 0 ||
        (mvdm_cpu_access_current()->flags() & CCPU_EFLAGS_VM) != 0) {
        *address_out = ((ULONG)selector << 4) + offset;
        return 1;
    }
    /* SIM32's original protected-mode rule reads the selected descriptor.
     * A visible selector does not identify one of the six mutable hidden
     * segment caches, so never choose a cache by register order here. */
    return mvdm_softpc_protected_address(selector, offset, address_out);
}

ULONG c_effective_addr(USHORT selector, ULONG offset)
{
    ULONG address;

    if (!mvdm_softpc_effective_address(selector, offset, &address)) return 0u;
    return address;
}

/* The original non-CCPU-facing SoftPC consumers, including base/video,
 * retain this public spelling.  It is a same-shaped facade over the selected
 * CCPU implementation, not a second address calculation or a host pointer
 * conversion. */
ULONG effective_addr(USHORT selector, ULONG offset)
{
    return c_effective_addr(selector, offset);
}
