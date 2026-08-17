#include "bochs.h"
#include "iodev/iodev.h"
#include "bx-mantle/pc_system.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"
#include "bx-mantle/bx_ntvdm_generic_ud_bridge.h"

// Product composition supplies this selector-blind bridge from bx-vdm.  This
// PIC-only fixture deliberately declines every #UD and never executes one.
extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *,
  struct bx_ntvdm_generic_ud_outcome_v1 *)
{
  return 0;
}

static int exercise_native_pic(void)
{
  bx_ntvdm_minimal_machine_c machine;
  if (machine.initialize(0x200000, 0x200000) != BX_NTVDM_MINIMAL_MACHINE_OK) return 1;
  if (bx_devices.pluginPicDevice == &bx_devices.stubPic) return 2;
  if (bx_devices.inp(0x0021, 1) != 0xff) return 3;
  bx_devices.outp(0x0021, 0xfe, 1);
  if (bx_devices.inp(0x0021, 1) != 0xfe) return 4;
  bx_devices.pluginPicDevice->raise_irq(0);
  if (bx_pc_system.IAC() != 0x08) return 5;
  bx_devices.outp(0x0020, 0x0b, 1);
  if (bx_devices.inp(0x0020, 1) != 0x01) return 6;
  bx_devices.outp(0x0020, 0x20, 1);
  if (bx_devices.inp(0x0020, 1) != 0x00) return 7;
  if (machine.cleanup() != BX_NTVDM_MINIMAL_MACHINE_OK) return 8;
  if (bx_devices.pluginPicDevice != &bx_devices.stubPic) return 9;
  return 0;
}

int main(void)
{
  int first = exercise_native_pic();
  if (first != 0) return first;
  return exercise_native_pic();
}