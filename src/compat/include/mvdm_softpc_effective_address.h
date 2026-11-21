#ifndef MVDM_SOFTPC_EFFECTIVE_ADDRESS_H
#define MVDM_SOFTPC_EFFECTIVE_ADDRESS_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Historical CCPU external form.  Both parameters and the result are guest
 * numeric values; this API never returns a native pointer. */
extern ULONG c_effective_addr(USHORT selector, ULONG offset);

/* Checked form distinguishes valid address zero from an invalid selector.
 * Explicit cdecl preserves the /Gz WOW32 consumer ABI. */
extern int __cdecl mvdm_softpc_effective_address(USHORT selector, ULONG offset,
    ULONG *address_out);
extern int __cdecl mvdm_softpc_protected_segment(USHORT selector, ULONG *base,
    ULONG *limit, ULONG *access);
/* SIM32's protected address contract is independent of CPU PE/VM. */
extern int __cdecl mvdm_softpc_protected_address(USHORT selector, ULONG offset,
    ULONG *address_out);

#ifdef __cplusplus
}
#endif

#endif
