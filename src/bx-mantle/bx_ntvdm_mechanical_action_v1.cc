/////////////////////////////////////////////////////////////////////////
//
// Ordinary-RAM action implementation for BX-MANTLE-068.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "bx-core/memory/memory.h"
#include "bx_ntvdm_mechanical_action_v1.h"

#include <string.h>

extern "C" void bx_ntvdm_mechanical_action_v1_clear(
  struct bx_ntvdm_mechanical_action_v1 *action)
{
  if (action == 0) return;
  memset(action, 0, sizeof(*action));
  action->magic = BX_NTVDM_MECHANICAL_ACTION_V1_MAGIC;
  action->abi_version = BX_NTVDM_MECHANICAL_ACTION_V1_VERSION;
  action->struct_bytes = sizeof(*action);
}

extern "C" int bx_ntvdm_mechanical_action_v1_valid(
  const struct bx_ntvdm_mechanical_action_v1 *action)
{
  uint32_t index;
  uint64_t total = 0;

  if (action == 0 ||
      action->magic != BX_NTVDM_MECHANICAL_ACTION_V1_MAGIC ||
      action->abi_version != BX_NTVDM_MECHANICAL_ACTION_V1_VERSION ||
      action->struct_bytes != sizeof(*action) || action->action_id == 0 ||
      (action->kind != BX_NTVDM_MECHANICAL_ACTION_V1_READ &&
       action->kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE) ||
      action->range_count == 0 ||
      action->range_count > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES ||
      action->payload_bytes == 0 ||
      action->payload_bytes > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES) return 0;

  for (index = 0; index < action->range_count; ++index) {
    const struct bx_ntvdm_mechanical_action_v1_range *range =
      &action->ranges[index];
    uint64_t payload_end = (uint64_t) range->payload_offset + range->byte_count;
    if (range->byte_count == 0 || payload_end > action->payload_bytes ||
        range->physical_address > UINT64_MAX - range->byte_count ||
        total > UINT64_MAX - range->byte_count) return 0;
    total += range->byte_count;
  }
  return total == action->payload_bytes;
}

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
