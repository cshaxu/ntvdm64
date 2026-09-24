#ifndef MVDM_SOFTPC_FAST_BOP_H
#define MVDM_SOFTPC_FAST_BOP_H

#include <stdint.h>

/* CCPU carrier for the original x86 FastBOP far-call ABI.  The returned
 * selector/offset names the native monitor endpoint; it is recognized before
 * guest descriptor/code fetch and resumes the original BOP dispatcher. */
int mvdm_softpc_fast_bop_prepare(uint16_t *offset, uint16_t *selector);
int mvdm_softpc_fast_bop_redirect(uint16_t selector, uint32_t offset);
void mvdm_softpc_fast_bop_sync_mode(void);
#define MVDM_FAST_BOP_INLINE 1
#define MVDM_FAST_BOP_WOW 2
#define MVDM_FAST_BOP_RETURN 3
uint32_t mvdm_softpc_fast_bop_dispatch_offset(void);
uint32_t mvdm_softpc_fast_bop_callback_offset(void);

#endif
