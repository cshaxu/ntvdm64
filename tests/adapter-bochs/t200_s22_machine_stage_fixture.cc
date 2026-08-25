#include "bochs.h"
#include "bochs-core/memory/memory.h"
#include "adapter-softpc/machine_stage.h"

#include <string.h>

int main()
{
  struct runtime_machine_stage_request request;
  struct runtime_machine_stage_entry entry;
  struct runtime_machine_stage_entry observed;
  struct runtime_machine_stage_execution_request execution;
  Bit8u preserved[4] = { 0, 0, 0, 0 };
  Bit8u startup[32];

  memset(startup, 0xaa, sizeof(startup));
  startup[0] = 0xf4u;
  runtime_machine_stage_request_clear(&request);
  if (runtime_machine_stage_begin(&request) !=
      RUNTIME_MACHINE_STAGE_REJECTED_INPUT ||
      runtime_machine_stage_active()) return 1;
  runtime_mechanical_action_clear(&request.initial_state_action);
  request.initial_state_action.action_id = 1u;
  request.initial_state_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  request.initial_state_action.range_count = 1u;
  request.initial_state_action.payload_bytes = sizeof(preserved);
  request.initial_state_action.ranges[0].physical_address = 0x714u;
  request.initial_state_action.ranges[0].byte_count = sizeof(preserved);
  memcpy(request.initial_state_action.payload, "\x10\x20\x30\x40",
    sizeof(preserved));

  runtime_mechanical_action_clear(&request.startup_action);
  request.startup_action.action_id = 2u;
  request.startup_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = sizeof(startup);
  request.startup_action.ranges[0].physical_address = 0x700u;
  request.startup_action.ranges[0].byte_count = sizeof(startup);
  memcpy(request.startup_action.payload, startup, sizeof(startup));
  request.preserved_state_address = 0x714u;
  request.preserved_state_bytes = sizeof(preserved);

  if (!runtime_machine_stage_request_valid(&request) ||
      runtime_machine_stage_begin(&request) != RUNTIME_MACHINE_STAGE_OK ||
      !runtime_machine_stage_active()) return 2;
  if (!bx_mem.copy_from_ordinary_ram(0x413u, 2u, preserved) ||
      preserved[0] != 0x80u || preserved[1] != 0x02u) return 10;
  if (runtime_machine_stage_begin(&request) !=
      RUNTIME_MACHINE_STAGE_REJECTED_ACTIVE) return 3;
  runtime_machine_stage_entry_clear(&entry);
  entry.cs = 0x70u;
  if (runtime_machine_stage_arm_real_mode_entry(&entry) !=
      RUNTIME_MACHINE_STAGE_OK ||
      runtime_machine_stage_copy_real_mode_entry(&observed) !=
      RUNTIME_MACHINE_STAGE_OK || observed.cs != entry.cs ||
      observed.eip != entry.eip) return 4;
  entry.eip = 0x10000u;
  if (runtime_machine_stage_arm_real_mode_entry(&entry) !=
      RUNTIME_MACHINE_STAGE_REJECTED_ENTRY ||
      runtime_machine_stage_copy_real_mode_entry(&observed) !=
      RUNTIME_MACHINE_STAGE_OK || observed.cs != 0x70u ||
      observed.eip != 0u) return 5;
  if (!bx_mem.copy_from_ordinary_ram(0x714u, sizeof(preserved), preserved) ||
      memcmp(preserved, "\x10\x20\x30\x40", sizeof(preserved)) != 0 ||
      !bx_mem.copy_from_ordinary_ram(0x700u, sizeof(preserved), preserved) ||
      memcmp(preserved, startup, sizeof(preserved)) != 0) return 6;
  runtime_machine_stage_execution_request_clear(&execution);
  execution.ips = 1000000u;
  execution.instruction_tick_budget = 1000000u;
  if (runtime_machine_stage_execute(&execution) !=
      RUNTIME_MACHINE_STAGE_EXECUTION_BUDGET) return 7;
  if (runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK ||
      runtime_machine_stage_active()) return 8;
  /* A source-built startup may have no external initial-state bytes.  That
   * must remain a selector-blind omission, not a rejected machine stage. */
  runtime_machine_stage_request_clear(&request);
  runtime_mechanical_action_clear(&request.startup_action);
  request.startup_action.action_id = 2u;
  request.startup_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = 1u;
  request.startup_action.ranges[0].physical_address = 0x700u;
  request.startup_action.ranges[0].byte_count = 1u;
  request.startup_action.payload[0] = 0xf4u;
  request.preserved_state_address = 0x714u;
  request.preserved_state_bytes = sizeof(preserved);
  if (!runtime_machine_stage_request_valid(&request) ||
      runtime_machine_stage_begin(&request) != RUNTIME_MACHINE_STAGE_OK ||
      runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK) return 9;
  return 0;
}
