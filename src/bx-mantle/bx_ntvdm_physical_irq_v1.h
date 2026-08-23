/////////////////////////////////////////////////////////////////////////
//
// Fixed, selector-blind physical-IRQ request for the active native machine.
// This transports only a physical 8259 line number.  It has no vector,
// guest address, callback, service, or host-process meaning.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_PHYSICAL_IRQ_V1_H
#define BX_NTVDM_PHYSICAL_IRQ_V1_H

#include <stdint.h>

#define BX_NTVDM_PHYSICAL_IRQ_V1_MAX 15u

#ifdef __cplusplus
extern "C" {
#endif

/* Returns nonzero after queuing a valid physical IRQ at the native PIC.
 * Delivery, acknowledgement and masking remain owned by the native PIC/CPU. */
int bx_ntvdm_mantle_request_physical_irq_v1(uint32_t irq);

#ifdef __cplusplus
}
#endif

#endif
