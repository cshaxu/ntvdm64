#include "bochs.h"
#include "bx-core/memory/memory.h"
#include "bx-mantle/bx_ntvdm_machine_stage_v1.h"

#include <string.h>

int main()
{
  struct bx_ntvdm_machine_stage_v1_request request;
  struct bx_ntvdm_machine_stage_v1_entry entry;
  struct bx_ntvdm_machine_stage_v1_entry observed;
  struct bx_ntvdm_machine_stage_v1_execution_request execution;
  Bit8u preserved[4] = { 0, 0, 0, 0 };
  Bit8u startup[32];

  memset(startup, 0xaa, sizeof(startup));
  startup[0] = 0xf4u;
  bx_ntvdm_machine_stage_v1_request_clear(&request);
  if (bx_ntvdm_machine_stage_v1_begin(&request) !=
      BX_NTVDM_MACHINE_STAGE_V1_REJECTED_INPUT ||
      bx_ntvdm_machine_stage_v1_active()) return 1;
  bx_ntvdm_mechanical_action_v1_clear(&request.initial_state_action);
  request.initial_state_action.action_id = 1u;
  request.initial_state_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  request.initial_state_action.range_count = 1u;
  request.initial_state_action.payload_bytes = sizeof(preserved);
  request.initial_state_action.ranges[0].physical_address = 0x714u;
  request.initial_state_action.ranges[0].byte_count = sizeof(preserved);
  memcpy(request.initial_state_action.payload, "\x10\x20\x30\x40",
    sizeof(preserved));

  bx_ntvdm_mechanical_action_v1_clear(&request.startup_action);
  request.startup_action.action_id = 2u;
  request.startup_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = sizeof(startup);
  request.startup_action.ranges[0].physical_address = 0x700u;
  request.startup_action.ranges[0].byte_count = sizeof(startup);
  memcpy(request.startup_action.payload, startup, sizeof(startup));
  request.preserved_state_address = 0x714u;
  request.preserved_state_bytes = sizeof(preserved);

  if (!bx_ntvdm_machine_stage_v1_request_valid(&request) ||
      bx_ntvdm_machine_stage_v1_begin(&request) != BX_NTVDM_MACHINE_STAGE_V1_OK ||
      !bx_ntvdm_machine_stage_v1_active()) return 2;
  if (!bx_mem.copy_from_ordinary_ram(0x413u, 2u, preserved) ||
      preserved[0] != 0x80u || preserved[1] != 0x02u) return 10;
  if (bx_ntvdm_machine_stage_v1_begin(&request) !=
      BX_NTVDM_MACHINE_STAGE_V1_REJECTED_ACTIVE) return 3;
  bx_ntvdm_machine_stage_v1_entry_clear(&entry);
  entry.cs = 0x70u;
  if (bx_ntvdm_machine_stage_v1_arm_real_mode_entry(&entry) !=
      BX_NTVDM_MACHINE_STAGE_V1_OK ||
      bx_ntvdm_machine_stage_v1_copy_real_mode_entry(&observed) !=
      BX_NTVDM_MACHINE_STAGE_V1_OK || observed.cs != entry.cs ||
      observed.eip != entry.eip) return 4;
  entry.eip = 0x10000u;
  if (bx_ntvdm_machine_stage_v1_arm_real_mode_entry(&entry) !=
      BX_NTVDM_MACHINE_STAGE_V1_REJECTED_ENTRY ||
      bx_ntvdm_machine_stage_v1_copy_real_mode_entry(&observed) !=
      BX_NTVDM_MACHINE_STAGE_V1_OK || observed.cs != 0x70u ||
      observed.eip != 0u) return 5;
  if (!bx_mem.copy_from_ordinary_ram(0x714u, sizeof(preserved), preserved) ||
      memcmp(preserved, "\x10\x20\x30\x40", sizeof(preserved)) != 0 ||
      !bx_mem.copy_from_ordinary_ram(0x700u, sizeof(preserved), preserved) ||
      memcmp(preserved, startup, sizeof(preserved)) != 0) return 6;
  bx_ntvdm_machine_stage_v1_execution_request_clear(&execution);
  execution.ips = 1000000u;
  execution.instruction_tick_budget = 1000000u;
  if (bx_ntvdm_machine_stage_v1_execute(&execution) !=
      BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET) return 7;
  if (bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK ||
      bx_ntvdm_machine_stage_v1_active()) return 8;
  /* A source-built startup may have no external initial-state bytes.  That
   * must remain a selector-blind omission, not a rejected machine stage. */
  bx_ntvdm_machine_stage_v1_request_clear(&request);
  bx_ntvdm_mechanical_action_v1_clear(&request.startup_action);
  request.startup_action.action_id = 2u;
  request.startup_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = 1u;
  request.startup_action.ranges[0].physical_address = 0x700u;
  request.startup_action.ranges[0].byte_count = 1u;
  request.startup_action.payload[0] = 0xf4u;
  request.preserved_state_address = 0x714u;
  request.preserved_state_bytes = sizeof(preserved);
  if (!bx_ntvdm_machine_stage_v1_request_valid(&request) ||
      bx_ntvdm_machine_stage_v1_begin(&request) != BX_NTVDM_MACHINE_STAGE_V1_OK ||
      bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK) return 9;
  return 0;
}
