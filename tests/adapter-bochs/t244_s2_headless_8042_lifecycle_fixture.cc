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

static int read8(struct runtime_port_action *a, uint16_t port)
{ runtime_port_action_initialize(a); a->kind=RUNTIME_PORT_ACTION_READ8; a->port=port; return runtime_machine_execute_port_action(a); }
static int write8(struct runtime_port_action *a, uint16_t port, uint8_t value)
{ runtime_port_action_initialize(a); a->kind=RUNTIME_PORT_ACTION_WRITE8; a->port=port; a->value=value; return runtime_machine_execute_port_action(a); }

static int exercise(void)
{
  adapter_bochs_minimal_machine_c machine;
  struct runtime_port_action a;
  if (machine.initialize(0x200000, 0x200000) != BX_MACHINE_MINIMAL_MACHINE_OK) return 1;
  if (read8(&a, 0x64u)) return 2;
  runtime_port_action_set_lifecycle_active(1u);
  bx_pc_system.initialize(1000000u);
  if (machine.compose_headless_8042() != BX_MACHINE_MINIMAL_MACHINE_OK) return 3;
  if (!read8(&a, 0x64u) || (a.value & 0x18u) != 0x18u) return 4;
  if (!write8(&a, 0x21u, 0xfdu)) return 5; // unmask ordinary IRQ1 only
  if (!write8(&a, 0x60u, 0xf2u)) return 6; // original identify command
  bx_pc_system.tickn(64u);
  if (bx_pc_system.IAC() != 0x09u) return 7;
  if (!read8(&a, 0x60u) || a.value != 0xfau) return 8;
  bx_pc_system.tickn(64u);
  if (!read8(&a, 0x60u) || a.value != 0xabu) return 9;
  bx_pc_system.tickn(64u);
  if (!read8(&a, 0x60u) || a.value != 0x41u) return 10;
  if (!write8(&a, 0x20u, 0x20u)) return 11;
  if (!write8(&a, 0x64u, 0xd4u) || !write8(&a, 0x60u, 0xffu)) return 12;
  bx_pc_system.tickn(64u);
  if (!read8(&a, 0x64u) || (a.value & 0x21u) != 0x00u) return 13;
  if (machine.cleanup() != BX_MACHINE_MINIMAL_MACHINE_OK) return 14;
  runtime_port_action_set_lifecycle_active(0u);
  if (read8(&a, 0x64u)) return 15;
  return 0;
}

int main(void)
{ int first = exercise(); return first ? first : exercise(); }
