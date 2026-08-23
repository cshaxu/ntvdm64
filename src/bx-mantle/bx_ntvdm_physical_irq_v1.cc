/////////////////////////////////////////////////////////////////////////
//
// Active-machine physical IRQ request implementation.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "iodev/iodev.h"
#include "bx_ntvdm_machine_stage_v1.h"
#include "bx_ntvdm_physical_irq_v1.h"

extern "C" int bx_ntvdm_mantle_request_physical_irq_v1(uint32_t irq)
{
  if (irq > BX_NTVDM_PHYSICAL_IRQ_V1_MAX ||
      !bx_ntvdm_machine_stage_v1_active() ||
      bx_devices.pluginPicDevice == &bx_devices.stubPic) return 0;

  /* The PIC, not this wrapper, owns cascade, masks, priority and CPU INTR. */
  bx_devices.pluginPicDevice->raise_irq((unsigned)irq);
  return 1;
}
