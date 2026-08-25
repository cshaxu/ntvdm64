#include "bochs.h"
#include "iodev/iodev.h"
#include "bochs-core/pc_system.h"
#include "adapter-bochs/minimal_machine.h"
#include "adapter-softpc/port_action.h"
#include "adapter-bop/generic_ud_bridge.h"

extern "C" int runtime_machine_generic_ud_bridge(
  const struct runtime_generic_ud_event *,
  struct runtime_generic_ud_outcome *)
{ return 0; }

static int read8(struct runtime_port_action *action, uint16_t port)
{ runtime_port_action_initialize(action); action->kind=RUNTIME_PORT_ACTION_READ8; action->port=port; return runtime_machine_execute_port_action(action); }
static int write8(struct runtime_port_action *action, uint16_t port, uint8_t value)
{ runtime_port_action_initialize(action); action->kind=RUNTIME_PORT_ACTION_WRITE8; action->port=port; action->value=value; return runtime_machine_execute_port_action(action); }

static int exercise_native_pic(void)
{
  adapter_bochs_minimal_machine_c machine;
  struct runtime_port_action action;
  if (read8(&action, 0x21u)) return 1;
  if (machine.initialize(0x200000, 0x200000) != BX_MACHINE_MINIMAL_MACHINE_OK) return 2;
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
  if (machine.cleanup() != BX_MACHINE_MINIMAL_MACHINE_OK) return 12;
  if (bx_devices.pluginPicDevice != &bx_devices.stubPic) return 13;
  if (read8(&action, 0x21u)) return 14;
  return 0;
}
int main(void)
{ int first = exercise_native_pic(); if (first != 0) return first; return exercise_native_pic(); }
