#include "bochs.h"
#include "adapter-softpc/bx_ntvdm_machine_stage_v1.h"
#include "adapter-softpc/bx_ntvdm_generic_ud_bridge.h"

#include <string.h>

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *,
  struct bx_ntvdm_generic_ud_outcome_v1 *)
{
  return 0;
}

static void write_action(struct bx_ntvdm_mechanical_action_v1 *action,
  uint32_t id, uint64_t address, const Bit8u *bytes, uint32_t byte_count)
{
  bx_ntvdm_mechanical_action_v1_clear(action);
  action->action_id = id;
  action->kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
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
  struct bx_ntvdm_machine_stage_v1_request request;
  struct bx_ntvdm_machine_stage_v1_entry entry;
  struct bx_ntvdm_machine_stage_v1_execution_request execution;

  bx_ntvdm_machine_stage_v1_request_clear(&request);
  write_action(&request.initial_state_action, 1u, 0x714u, preserved,
    sizeof(preserved));
  write_action(&request.startup_action, 2u, 0x700u, loop, sizeof(loop));
  request.preserved_state_address = 0x714u;
  request.preserved_state_bytes = sizeof(preserved);
  if (!bx_ntvdm_machine_stage_v1_request_valid(&request) ||
      bx_ntvdm_machine_stage_v1_begin(&request) != BX_NTVDM_MACHINE_STAGE_V1_OK)
    return 1;
  bx_ntvdm_machine_stage_v1_entry_clear(&entry);
  entry.cs = 0x70u;
  if (bx_ntvdm_machine_stage_v1_arm_real_mode_entry(&entry) !=
      BX_NTVDM_MACHINE_STAGE_V1_OK) return 2;
  bx_ntvdm_machine_stage_v1_execution_request_clear(&execution);
  execution.ips = 1000000u;
  execution.instruction_tick_budget = 16u;
  if (bx_ntvdm_machine_stage_v1_execute(&execution) !=
      BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET) return 3;
  return bx_ntvdm_machine_stage_v1_reset() == BX_NTVDM_MACHINE_STAGE_V1_OK ? 0 : 4;
}
