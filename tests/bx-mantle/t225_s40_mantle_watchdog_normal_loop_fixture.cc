#include "bochs.h"
#include "adapter-softpc/machine_stage.h"
#include "adapter-bop/generic_ud_bridge.h"

#include <string.h>

extern "C" int runtime_mantle_generic_ud_bridge_v1(
  const struct runtime_generic_ud_event_v1 *,
  struct runtime_generic_ud_outcome_v1 *)
{
  return 0;
}

static void write_action(struct runtime_mechanical_action_v1 *action,
  uint32_t id, uint64_t address, const Bit8u *bytes, uint32_t byte_count)
{
  runtime_mechanical_action_v1_clear(action);
  action->action_id = id;
  action->kind = RUNTIME_MECHANICAL_ACTION_V1_WRITE;
  action->range_count = 1u;
  action->payload_bytes = byte_count;
  action->ranges[0].physical_address = address;
  action->ranges[0].byte_count = byte_count;
  memcpy(action->payload, bytes, byte_count);
}

int main()
{
  static const Bit8u preserved[] = { 0x10u, 0x20u, 0x30u, 0x40u };
  static const Bit8u loop[] = { 0xebu, 0xfeu };
  struct runtime_machine_stage_v1_request request;
  struct runtime_machine_stage_v1_entry entry;
  struct runtime_machine_stage_v1_execution_request execution;

  runtime_machine_stage_v1_request_clear(&request);
  write_action(&request.initial_state_action, 1u, 0x714u, preserved,
    sizeof(preserved));
  write_action(&request.startup_action, 2u, 0x700u, loop, sizeof(loop));
  request.preserved_state_address = 0x714u;
  request.preserved_state_bytes = sizeof(preserved);
  if (!runtime_machine_stage_v1_request_valid(&request) ||
      runtime_machine_stage_v1_begin(&request) != RUNTIME_MACHINE_STAGE_V1_OK)
    return 1;
  runtime_machine_stage_v1_entry_clear(&entry);
  entry.cs = 0x70u;
  if (runtime_machine_stage_v1_arm_real_mode_entry(&entry) !=
      RUNTIME_MACHINE_STAGE_V1_OK) return 2;
  runtime_machine_stage_v1_execution_request_clear(&execution);
  execution.ips = 1000000u;
  execution.instruction_tick_budget = 16u;
  if (runtime_machine_stage_v1_execute(&execution) !=
      RUNTIME_MACHINE_STAGE_V1_EXECUTION_BUDGET) return 3;
  return runtime_machine_stage_v1_reset() == RUNTIME_MACHINE_STAGE_V1_OK ? 0 : 4;
}
