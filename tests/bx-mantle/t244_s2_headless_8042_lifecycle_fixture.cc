#include "bochs.h"
#include "iodev/iodev.h"
#include "bochs-core/pc_system.h"
#include "adapter-bochs/minimal_machine.h"
#include "adapter-softpc/port_action.h"
#include "adapter-bop/generic_ud_bridge.h"

extern "C" int runtime_mantle_generic_ud_bridge_v1(
  const struct runtime_generic_ud_event_v1 *,
  struct runtime_generic_ud_outcome_v1 *)
{ return 0; }

static int read8(struct runtime_port_action_v1 *a, uint16_t port)
{ runtime_port_action_v1_initialize(a); a->kind=RUNTIME_PORT_ACTION_V1_READ8; a->port=port; return runtime_mantle_execute_port_action_v1(a); }
static int write8(struct runtime_port_action_v1 *a, uint16_t port, uint8_t value)
{ runtime_port_action_v1_initialize(a); a->kind=RUNTIME_PORT_ACTION_V1_WRITE8; a->port=port; a->value=value; return runtime_mantle_execute_port_action_v1(a); }

static int exercise(void)
{
  bx_mantle_minimal_machine_c machine;
  struct runtime_port_action_v1 a;
  if (machine.initialize(0x200000, 0x200000) != BX_MANTLE_MINIMAL_MACHINE_OK) return 1;
  if (read8(&a, 0x64u)) return 2;
  bx_pc_system.initialize(1000000u);
  if (machine.compose_headless_8042() != BX_MANTLE_MINIMAL_MACHINE_OK) return 3;
  if (!read8(&a, 0x64u) || (a.value & 0x18u) != 0x18u) return 4;
  if (!write8(&a, 0x21u, 0xfdu)) return 5; // unmask ordinary IRQ1 only
  if (!write8(&a, 0x60u, 0xf2u)) return 6; // original identify command
  bx_pc_system.tickn(64u);
  if (bx_pc_system.IAC() != 0x09u) return 7;
  if (!read8(&a, 0x60u) || a.value != 0xfau) return 8;
  if (!write8(&a, 0x20u, 0x20u)) return 9;
  if (!write8(&a, 0x64u, 0xd4u) || !write8(&a, 0x60u, 0xffu)) return 10;
  bx_pc_system.tickn(64u);
  if (!read8(&a, 0x64u) || (a.value & 0x21u) != 0x00u) return 11;
  if (machine.cleanup() != BX_MANTLE_MINIMAL_MACHINE_OK) return 12;
  if (read8(&a, 0x64u)) return 13;
  return 0;
}

int main(void)
{ int first = exercise(); return first ? first : exercise(); }
