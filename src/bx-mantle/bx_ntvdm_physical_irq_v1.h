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

/* Synchronous execution-thread request.  Delivery, acknowledgement and
 * masking remain owned by the native PIC/CPU. */
int bx_ntvdm_mantle_request_physical_irq_v1(uint32_t irq);

/* Thread-safe producer operation.  It publishes only a physical line number;
 * it never touches a Bochs object.  The active-stage timer consumes it on the
 * CPU thread through bx_ntvdm_mantle_drain_posted_physical_irqs_v1(). */
int bx_ntvdm_mantle_post_physical_irq_v1(uint32_t irq);

/* CPU-thread-only consumer used by the finite machine-stage timer.  Returns
 * the number of accepted lines delivered to the existing native PIC. */
uint32_t bx_ntvdm_mantle_drain_posted_physical_irqs_v1(void);

/* Clears unconsumed producer state during the active-machine lifecycle. */
void bx_ntvdm_mantle_clear_posted_physical_irqs_v1(void);

#ifdef __cplusplus
}
#endif

#endif
