/////////////////////////////////////////////////////////////////////////
//
// Ordinary-RAM action implementation for BX-MANTLE-068.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "bx-core/memory/memory.h"
#include "bx_ntvdm_mechanical_action_v1.h"

extern "C" int bx_ntvdm_mantle_execute_mechanical_action_v1(
  struct bx_ntvdm_mechanical_action_v1 *action)
{
  uint32_t index;

  if (!bx_ntvdm_mechanical_action_v1_valid(action)) return 0;
  for (index = 0; index < action->range_count; ++index) {
    const struct bx_ntvdm_mechanical_action_v1_range *range =
      &action->ranges[index];
    bx_bool accessible = action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_READ ?
      bx_mem.ordinary_ram_readable(range->physical_address, range->byte_count) :
      bx_mem.ordinary_ram_writable(range->physical_address, range->byte_count);
    if (!accessible) return 0;
  }
  for (index = 0; index < action->range_count; ++index) {
    const struct bx_ntvdm_mechanical_action_v1_range *range =
      &action->ranges[index];
    uint8_t *bytes = action->payload + range->payload_offset;
    bx_bool copied = action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_READ ?
      bx_mem.copy_from_ordinary_ram(range->physical_address, range->byte_count, bytes) :
      bx_mem.copy_to_ordinary_ram(range->physical_address, range->byte_count, bytes);
    if (!copied) return 0;
  }
  return 1;
}
