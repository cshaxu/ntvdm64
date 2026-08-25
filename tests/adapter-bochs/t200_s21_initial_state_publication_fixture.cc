#include "bochs.h"
#include "adapter-softpc/mechanical_action.h"
#include "adapter-bochs/minimal_machine.h"
#include "app/initial_state_action.h"

#include <string.h>

int main()
{
  runtime_initial_state state;
  struct runtime_mechanical_action action;
  Bit8u bytes[5] = { 0, 0, 0, 0, 0 };
  Bit8u original[3] = { 0, 0, 0 };
  adapter_bochs_minimal_machine_c machine;

  runtime_initial_state_clear(&state);
  state.disposition = RUNTIME_INITIAL_STATE_PRESENT;
  state.range_count = 2u;
  state.payload_bytes = sizeof(bytes);
  state.evidence_sha256[0] = 1u;
  runtime_guest_write_initialize(&state.ranges[0], 0x40u, 2u, 0u);
  runtime_guest_write_initialize(&state.ranges[1], 0x496u, 3u, 2u);
  bytes[0] = 0x10; bytes[1] = 0x20; bytes[2] = 0x30;
  bytes[3] = 0x40; bytes[4] = 0x50;
  memcpy(state.payload, bytes, sizeof(bytes));
  if (!runtime_initial_state_action_prepare(&state, &action)) return 1;
  if (machine.initialize(0x100000, 0x100000) != BX_MACHINE_MINIMAL_MACHINE_OK)
    return 2;

  /* The generic machine operation has no profile or service knowledge. */
  if (!runtime_machine_execute_mechanical_action(&action) ||
      !bx_mem.copy_from_ordinary_ram(0x40u, 2u, original) ||
      original[0] != 0x10 || original[1] != 0x20 ||
      !bx_mem.copy_from_ordinary_ram(0x496u, 3u, original) ||
      original[0] != 0x30 || original[1] != 0x40 || original[2] != 0x50)
    return 3;

  /* A later inaccessible range is rejected before the earlier write begins. */
  action.ranges[0].physical_address = 0x80u;
  action.ranges[1].physical_address = 0xfffffu;
  if (runtime_machine_execute_mechanical_action(&action) ||
      !bx_mem.copy_from_ordinary_ram(0x80u, 2u, original) ||
      original[0] != 0u || original[1] != 0u)
    return 4;

  return machine.cleanup() == BX_MACHINE_MINIMAL_MACHINE_OK ? 0 : 5;
}