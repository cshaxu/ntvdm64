/////////////////////////////////////////////////////////////////////////
//
// Active-machine physical IRQ request implementation.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "iodev/iodev.h"
#include "machine_stage.h"
#include "physical_irq.h"

#include <intrin.h>

static volatile long runtime_physical_irq_posted_mask = 0;

static int runtime_machine_physical_irq_valid(uint32_t irq)
{
  return irq <= RUNTIME_PHYSICAL_IRQ_MAX;
}

extern "C" int runtime_machine_request_physical_irq(uint32_t irq)
{
  if (!runtime_machine_physical_irq_valid(irq) ||
      !runtime_machine_stage_active() ||
      bx_devices.pluginPicDevice == &bx_devices.stubPic) return 0;

  /* The PIC, not this wrapper, owns cascade, masks, priority and CPU INTR. */
  bx_devices.pluginPicDevice->raise_irq((unsigned)irq);
  return 1;
}

extern "C" int runtime_machine_post_physical_irq(uint32_t irq)
{
  if (!runtime_machine_physical_irq_valid(irq) ||
      !runtime_machine_stage_active()) return 0;
  _InterlockedOr(&runtime_physical_irq_posted_mask, (long)(1u << irq));
  return 1;
}

extern "C" uint32_t runtime_machine_drain_posted_physical_irqs(void)
{
  uint32_t mask = (uint32_t)_InterlockedExchange(&runtime_physical_irq_posted_mask, 0);
  uint32_t irq, delivered = 0u;
  for (irq = 0u; irq <= RUNTIME_PHYSICAL_IRQ_MAX; ++irq) {
    if ((mask & (1u << irq)) != 0u &&
        runtime_machine_request_physical_irq(irq)) ++delivered;
  }
  return delivered;
}

extern "C" void runtime_machine_clear_posted_physical_irqs(void)
{
  (void)_InterlockedExchange(&runtime_physical_irq_posted_mask, 0);
}
