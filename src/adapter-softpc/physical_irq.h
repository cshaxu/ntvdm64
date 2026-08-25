/////////////////////////////////////////////////////////////////////////
//
// Fixed, selector-blind physical-IRQ request for the active native machine.
// This transports only a physical 8259 line number.  It has no vector,
// guest address, callback, service, or host-process meaning.
//
/////////////////////////////////////////////////////////////////////////

#ifndef RUNTIME_PHYSICAL_IRQ_H
#define RUNTIME_PHYSICAL_IRQ_H

#include <stdint.h>

#define RUNTIME_PHYSICAL_IRQ_MAX 15u

#ifdef __cplusplus
extern "C" {
#endif

/* Synchronous execution-thread request.  Delivery, acknowledgement and
 * masking remain owned by the native PIC/CPU. */
int runtime_machine_request_physical_irq(uint32_t irq);

/* Thread-safe producer operation.  It publishes only a physical line number;
 * it never touches a Bochs object.  The active-stage timer consumes it on the
 * CPU thread through runtime_machine_drain_posted_physical_irqs(). */
int runtime_machine_post_physical_irq(uint32_t irq);

/* CPU-thread-only consumer used by the finite machine-stage timer.  Returns
 * the number of accepted lines delivered to the existing native PIC. */
uint32_t runtime_machine_drain_posted_physical_irqs(void);

/* Clears unconsumed producer state during the active-machine lifecycle. */
void runtime_machine_clear_posted_physical_irqs(void);

#ifdef __cplusplus
}
#endif

#endif
