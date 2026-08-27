#include "adapter-mvdm-host-out/softpc/machine_stage.h"
#include "adapter-mvdm-host-out/softpc/mechanical_action.h"

#include <string.h>

/* The formal CPU5 projection composes its generic-UD consumer externally.
 * This RAM-only fixture deliberately supplies a decline-only stub so it can
 * validate machine-stage memory transport without selecting a BOP component. */
extern "C" int runtime_machine_generic_ud_bridge(
  const struct runtime_generic_ud_event *,
  struct runtime_generic_ud_outcome *)
{ return 0; }

static void write_action(struct runtime_mechanical_action *action,
  uint32_t id, uint64_t address, uint8_t value)
{
  runtime_mechanical_action_clear(action);
  action->action_id = id;
  action->kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  action->range_count = 1u;
  action->payload_bytes = 1u;
  action->ranges[0].physical_address = address;
  action->ranges[0].byte_count = 1u;
  action->payload[0] = value;
}

int main()
{
  struct runtime_machine_stage_request request;
  uint8_t source = 0x5au, value = 0u, replacement = 0xa5u;

  if (runtime_machine_checked_ram_read(0x200u, &value, 1u) ||
      runtime_machine_checked_ram_write(0x200u, &source, 1u)) return 1;
  runtime_machine_stage_request_clear(&request);
  write_action(&request.initial_state_action, 1u, 0x40u, 0u);
  write_action(&request.startup_action, 2u, 0x100u, 0xf4u);
  request.preserved_state_address = 0x40u;
  request.preserved_state_bytes = 1u;
  if (runtime_machine_stage_begin(&request) != RUNTIME_MACHINE_STAGE_OK ||
      !runtime_machine_stage_active()) return 2;
  if (!runtime_machine_checked_ram_write(0x200u, &source, 1u) ||
      !runtime_machine_checked_ram_read(0x200u, &value, 1u) ||
      value != source) return 3;
  if (runtime_machine_checked_ram_write(0x100000u, &replacement, 1u) ||
      !runtime_machine_checked_ram_read(0x200u, &value, 1u) ||
      value != source) return 4;
  if (runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK ||
      runtime_machine_checked_ram_read(0x200u, &value, 1u)) return 5;
  return 0;
}
