#include "bochs.h"
#include "iodev/iodev.h"
#include "bx-mantle/pc_system.h"
#include "bx-mantle/minimal_machine.h"
#include "adapter-softpc/bx_ntvdm_port_action_v1.h"
#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *,
  struct bx_ntvdm_generic_ud_outcome_v1 *)
{ return 0; }

static int read8(struct bx_ntvdm_port_action_v1 *action, uint16_t port)
{ bx_ntvdm_port_action_v1_initialize(action); action->kind=BX_NTVDM_PORT_ACTION_V1_READ8; action->port=port; return bx_ntvdm_mantle_execute_port_action_v1(action); }
static int write8(struct bx_ntvdm_port_action_v1 *action, uint16_t port, uint8_t value)
{ bx_ntvdm_port_action_v1_initialize(action); action->kind=BX_NTVDM_PORT_ACTION_V1_WRITE8; action->port=port; action->value=value; return bx_ntvdm_mantle_execute_port_action_v1(action); }

static int exercise_native_pic(void)
{
  bx_mantle_minimal_machine_c machine;
  struct bx_ntvdm_port_action_v1 action;
  if (read8(&action, 0x21u)) return 1;
  if (machine.initialize(0x200000, 0x200000) != BX_MANTLE_MINIMAL_MACHINE_OK) return 2;
  if (bx_devices.pluginPicDevice == &bx_devices.stubPic) return 3;
  if (!read8(&action, 0x21u) || action.value != 0xffu) return 4;
  if (!write8(&action, 0x21u, 0xfeu)) return 5;
  if (!read8(&action, 0x21u) || action.value != 0xfeu) return 6;
  bx_devices.pluginPicDevice->raise_irq(0);
  if (bx_pc_system.IAC() != 0x08) return 7;
  if (!write8(&action, 0x20u, 0x0bu)) return 8;
  if (!read8(&action, 0x20u) || action.value != 0x01u) return 9;
  if (!write8(&action, 0x20u, 0x20u)) return 10;
  if (!read8(&action, 0x20u) || action.value != 0x00u) return 11;
  if (machine.cleanup() != BX_MANTLE_MINIMAL_MACHINE_OK) return 12;
  if (bx_devices.pluginPicDevice != &bx_devices.stubPic) return 13;
  if (read8(&action, 0x21u)) return 14;
  return 0;
}
int main(void)
{ int first = exercise_native_pic(); if (first != 0) return first; return exercise_native_pic(); }