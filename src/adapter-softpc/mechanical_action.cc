/////////////////////////////////////////////////////////////////////////
//
// Ordinary-RAM action implementation for BX-MANTLE-068.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "bx-core/memory/memory.h"
#include "mechanical_action.h"
#include "machine_stage.h"

static int runtime_mantle_checked_ram_range_valid(uint64_t physical_address,
  const uint8_t *bytes, uint32_t byte_count, int write)
{
  if (!runtime_machine_stage_v1_active() || bytes == 0 || byte_count == 0u ||
      physical_address > UINT64_MAX - byte_count) return 0;
  return write ? bx_mem.ordinary_ram_writable(physical_address, byte_count) :
    bx_mem.ordinary_ram_readable(physical_address, byte_count);
}

extern "C" int runtime_mantle_checked_ram_read_v1(uint64_t physical_address,
  uint8_t *bytes, uint32_t byte_count)
{
  return runtime_mantle_checked_ram_range_valid(physical_address, bytes,
    byte_count, 0) && bx_mem.copy_from_ordinary_ram(physical_address,
      byte_count, bytes);
}

extern "C" int runtime_mantle_checked_ram_write_v1(uint64_t physical_address,
  const uint8_t *bytes, uint32_t byte_count)
{
  return runtime_mantle_checked_ram_range_valid(physical_address, bytes,
    byte_count, 1) && bx_mem.copy_to_ordinary_ram(physical_address,
      byte_count, bytes);
}

extern "C" int runtime_mantle_execute_mechanical_action_v1(
  struct runtime_mechanical_action_v1 *action)
{
  uint32_t index;

  if (!runtime_mechanical_action_v1_valid(action)) return 0;
  for (index = 0; index < action->range_count; ++index) {
    const struct runtime_mechanical_action_v1_range *range =
      &action->ranges[index];
    bx_bool accessible = action->kind == RUNTIME_MECHANICAL_ACTION_V1_READ ?
      bx_mem.ordinary_ram_readable(range->physical_address, range->byte_count) :
      bx_mem.ordinary_ram_writable(range->physical_address, range->byte_count);
    if (!accessible) return 0;
  }
  if (action->kind == RUNTIME_MECHANICAL_ACTION_V1_PREFLIGHT_WRITE) return 1;
  for (index = 0; index < action->range_count; ++index) {
    const struct runtime_mechanical_action_v1_range *range =
      &action->ranges[index];
    uint8_t *bytes = action->payload + range->payload_offset;
    bx_bool copied = action->kind == RUNTIME_MECHANICAL_ACTION_V1_READ ?
      bx_mem.copy_from_ordinary_ram(range->physical_address, range->byte_count, bytes) :
      bx_mem.copy_to_ordinary_ram(range->physical_address, range->byte_count, bytes);
    if (!copied) return 0;
  }
  return 1;
}
