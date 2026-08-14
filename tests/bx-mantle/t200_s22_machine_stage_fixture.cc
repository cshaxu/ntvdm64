#include "bochs.h"
#include "bx-core/memory/memory.h"
#include "bx-mantle/bx_ntvdm_machine_stage_v1.h"

#include <string.h>

int main()
{
  struct bx_ntvdm_machine_stage_v1_request request;
  Bit8u preserved[4] = { 0, 0, 0, 0 };
  Bit8u startup[32];

  memset(startup, 0xaa, sizeof(startup));
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
  if (bx_ntvdm_machine_stage_v1_begin(&request) !=
      BX_NTVDM_MACHINE_STAGE_V1_REJECTED_ACTIVE) return 3;
  if (!bx_mem.copy_from_ordinary_ram(0x714u, sizeof(preserved), preserved) ||
      memcmp(preserved, "\x10\x20\x30\x40", sizeof(preserved)) != 0 ||
      !bx_mem.copy_from_ordinary_ram(0x700u, sizeof(preserved), preserved) ||
      memcmp(preserved, startup, sizeof(preserved)) != 0) return 4;
  if (bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK ||
      bx_ntvdm_machine_stage_v1_active()) return 5;
  return 0;
}
