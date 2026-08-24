#include "adapter-softpc/bx_ntvdm_machine_stage_v1.h"
#include "adapter-softpc/bx_ntvdm_mechanical_action_v1.h"

#include <string.h>

/* The formal CPU5 projection composes its generic-UD consumer externally.
 * This RAM-only fixture deliberately supplies a decline-only stub so it can
 * validate machine-stage memory transport without selecting a BOP component. */
extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *,
  struct bx_ntvdm_generic_ud_outcome_v1 *)
{ return 0; }

static void write_action(struct bx_ntvdm_mechanical_action_v1 *action,
  uint32_t id, uint64_t address, uint8_t value)
{
  bx_ntvdm_mechanical_action_v1_clear(action);
  action->action_id = id;
  action->kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  action->range_count = 1u;
  action->payload_bytes = 1u;
  action->ranges[0].physical_address = address;
  action->ranges[0].byte_count = 1u;
  action->payload[0] = value;
}

int main()
{
  struct bx_ntvdm_machine_stage_v1_request request;
  uint8_t source = 0x5au, value = 0u, replacement = 0xa5u;

  if (bx_ntvdm_mantle_checked_ram_read_v1(0x200u, &value, 1u) ||
      bx_ntvdm_mantle_checked_ram_write_v1(0x200u, &source, 1u)) return 1;
  bx_ntvdm_machine_stage_v1_request_clear(&request);
  write_action(&request.initial_state_action, 1u, 0x40u, 0u);
  write_action(&request.startup_action, 2u, 0x100u, 0xf4u);
  request.preserved_state_address = 0x40u;
  request.preserved_state_bytes = 1u;
  if (bx_ntvdm_machine_stage_v1_begin(&request) != BX_NTVDM_MACHINE_STAGE_V1_OK ||
      !bx_ntvdm_machine_stage_v1_active()) return 2;
  if (!bx_ntvdm_mantle_checked_ram_write_v1(0x200u, &source, 1u) ||
      !bx_ntvdm_mantle_checked_ram_read_v1(0x200u, &value, 1u) ||
      value != source) return 3;
  if (bx_ntvdm_mantle_checked_ram_write_v1(0x100000u, &replacement, 1u) ||
      !bx_ntvdm_mantle_checked_ram_read_v1(0x200u, &value, 1u) ||
      value != source) return 4;
  if (bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK ||
      bx_ntvdm_mantle_checked_ram_read_v1(0x200u, &value, 1u)) return 5;
  return 0;
}
