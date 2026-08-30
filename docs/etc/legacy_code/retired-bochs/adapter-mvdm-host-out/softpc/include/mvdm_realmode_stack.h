#ifndef MVDM_REALMODE_STACK_H
#define MVDM_REALMODE_STACK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Same-shaped real-mode stack operations used where original MVDM source
 * formerly borrowed an unbounded Sim32GetVDMPointer alias.  They retain only
 * source numeric frame data and acquire fresh session-owned guest leases. */
int mvdm_realmode_push_interrupt(uint16_t interrupt_number,
    uint32_t return_bop_far);
int mvdm_realmode_simulate_iret_cf(void);

#ifdef __cplusplus
}
#endif

#endif
