#ifndef MVDM_SOFTPC_EFFECTIVE_ADDRESS_H
#define MVDM_SOFTPC_EFFECTIVE_ADDRESS_H

#include <insignia.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Historical CCPU external form.  Both parameters and the result are guest
 * numeric values; this API never returns a native pointer. */
IMPORT IU32 c_effective_addr IPT2(IU16, selector, IU32, offset);

/* Checked form for host consumers that must distinguish a valid linear
 * address zero from an invalid protected-mode selector. */
IMPORT int mvdm_softpc_effective_address IPT3(IU16, selector, IU32, offset,
                                              IU32 *, address_out);

#ifdef __cplusplus
}
#endif

#endif
