/////////////////////////////////////////////////////////////////////////
//
// Ordinary-RAM action implementation for BX-MANTLE-068.
//
/////////////////////////////////////////////////////////////////////////

#include "mechanical_action.h"
#include "adapter-bochs/machine_facade.h"
#include "machine_stage.h"

static int runtime_mantle_checked_ram_range_valid(uint64_t physical_address,
  const uint8_t *bytes, uint32_t byte_count, int write)
{
  if (!runtime_machine_stage_v1_active() || bytes == 0 || byte_count == 0u ||
      physical_address > UINT64_MAX - byte_count) return 0;
  return write ? machine_facade_v1_memory_writable(physical_address,
    byte_count) : machine_facade_v1_memory_readable(physical_address,
    byte_count);
}

extern "C" int runtime_mantle_checked_ram_read_v1(uint64_t physical_address,
  uint8_t *bytes, uint32_t byte_count)
{
  return runtime_mantle_checked_ram_range_valid(physical_address, bytes,
    byte_count, 0) && machine_facade_v1_memory_read(physical_address,
      byte_count, bytes);
}

extern "C" int runtime_mantle_checked_ram_write_v1(uint64_t physical_address,
  const uint8_t *bytes, uint32_t byte_count)
{
  return runtime_mantle_checked_ram_range_valid(physical_address, bytes,
    byte_count, 1) && machine_facade_v1_memory_write(physical_address,
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
    int accessible = action->kind == RUNTIME_MECHANICAL_ACTION_V1_READ ?
      machine_facade_v1_memory_readable(range->physical_address,
        range->byte_count) : machine_facade_v1_memory_writable(
          range->physical_address, range->byte_count);
    if (!accessible) return 0;
  }
  if (action->kind == RUNTIME_MECHANICAL_ACTION_V1_PREFLIGHT_WRITE) return 1;
  for (index = 0; index < action->range_count; ++index) {
    const struct runtime_mechanical_action_v1_range *range =
      &action->ranges[index];
    uint8_t *bytes = action->payload + range->payload_offset;
    int copied = action->kind == RUNTIME_MECHANICAL_ACTION_V1_READ ?
      machine_facade_v1_memory_read(range->physical_address,
        range->byte_count, bytes) : machine_facade_v1_memory_write(
          range->physical_address, range->byte_count, bytes);
    if (!copied) return 0;
  }
  return 1;
}
