#include "bochs.h"
#include "bochs-core/memory/memory.h"
#include "ordinary_ram_reservation.h"

#include <string.h>

struct runtime_ordinary_ram_reservation_v1_record {
  uint32_t opaque_id;
  uint32_t address;
  uint32_t byte_count;
};

static uint32_t runtime_ordinary_ram_reservation_v1_lifecycle_active;
static uint32_t runtime_ordinary_ram_reservation_v1_base;
static uint32_t runtime_ordinary_ram_reservation_v1_bytes;
static uint32_t runtime_ordinary_ram_reservation_v1_next_id;
static struct runtime_ordinary_ram_reservation_v1_record
  runtime_ordinary_ram_reservation_v1_records[
    RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAX_RECORDS];

static void runtime_ordinary_ram_reservation_v1_reset_state(void)
{
  runtime_ordinary_ram_reservation_v1_base = 0u;
  runtime_ordinary_ram_reservation_v1_bytes = 0u;
  runtime_ordinary_ram_reservation_v1_next_id = 1u;
  memset(runtime_ordinary_ram_reservation_v1_records, 0,
    sizeof(runtime_ordinary_ram_reservation_v1_records));
}

static int runtime_ordinary_ram_reservation_v1_power_of_two(uint32_t value)
{
  return value != 0u && (value & (value - 1u)) == 0u;
}

static int runtime_ordinary_ram_reservation_v1_interval_valid(uint64_t base,
  uint64_t bytes)
{
  return (base == 0u && bytes == 0u) ||
    (base >= UINT64_C(0x100000) && base <= UINT64_C(0xffffffff) &&
     bytes != 0u && bytes <= UINT64_C(0xffffffff) &&
     base % UINT64_C(0x10000) == 0u && bytes % UINT64_C(0x10000) == 0u &&
     base + bytes > base && base + bytes <= UINT64_C(0x1000000));
}

static int runtime_ordinary_ram_reservation_v1_span_ordinary(uint32_t address,
  uint32_t byte_count)
{
  return byte_count != 0u && (uint64_t) address + byte_count <= UINT64_C(0x100000000) &&
    bx_mem.ordinary_ram_readable((bx_phy_address) address, byte_count) &&
    bx_mem.ordinary_ram_writable((bx_phy_address) address, byte_count);
}

extern "C" void runtime_ordinary_ram_reservation_v1_clear(
  struct runtime_ordinary_ram_reservation_v1 *action)
{
  if (action == 0) return;
  memset(action, 0, sizeof(*action));
  action->magic = RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAGIC;
  action->abi_version = RUNTIME_ORDINARY_RAM_RESERVATION_V1_VERSION;
  action->struct_bytes = sizeof(*action);
  action->status = RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_INPUT;
}

extern "C" int runtime_ordinary_ram_reservation_v1_valid(
  const struct runtime_ordinary_ram_reservation_v1 *action)
{
  if (action == 0 || action->magic != RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAGIC ||
      action->abi_version != RUNTIME_ORDINARY_RAM_RESERVATION_V1_VERSION ||
      action->struct_bytes != sizeof(*action)) return 0;
  if (action->kind == RUNTIME_ORDINARY_RAM_RESERVATION_V1_ALLOCATE)
    return action->byte_count != 0u &&
      action->alignment_bytes >= RUNTIME_ORDINARY_RAM_RESERVATION_V1_MIN_ALIGNMENT &&
      action->alignment_bytes <= RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAX_ALIGNMENT &&
      runtime_ordinary_ram_reservation_v1_power_of_two(action->alignment_bytes) &&
      action->opaque_id == 0u && action->address == 0u;
  return action->kind == RUNTIME_ORDINARY_RAM_RESERVATION_V1_RELEASE &&
    action->byte_count == 0u && action->alignment_bytes == 0u &&
    action->opaque_id != 0u && action->address == 0u;
}

extern "C" uint32_t runtime_ordinary_ram_reservation_v1_configure(
  uint64_t base, uint64_t bytes)
{
  if (runtime_ordinary_ram_reservation_v1_lifecycle_active)
    return RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_LIFECYCLE;
  runtime_ordinary_ram_reservation_v1_reset_state();
  if (!runtime_ordinary_ram_reservation_v1_interval_valid(base, bytes))
    return RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_CONFIGURATION;
  if (bytes != 0u && !runtime_ordinary_ram_reservation_v1_span_ordinary(
      (uint32_t) base, (uint32_t) bytes))
    return RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_MEMORY;
  runtime_ordinary_ram_reservation_v1_base = (uint32_t) base;
  runtime_ordinary_ram_reservation_v1_bytes = (uint32_t) bytes;
  return RUNTIME_ORDINARY_RAM_RESERVATION_V1_OK;
}

extern "C" void runtime_ordinary_ram_reservation_v1_set_lifecycle_active(
  uint32_t active)
{
  runtime_ordinary_ram_reservation_v1_lifecycle_active = active == 1u ? 1u : 0u;
  if (!runtime_ordinary_ram_reservation_v1_lifecycle_active)
    runtime_ordinary_ram_reservation_v1_reset_state();
}

extern "C" uint32_t runtime_mantle_execute_ordinary_ram_reservation_v1(
  struct runtime_ordinary_ram_reservation_v1 *action)
{
  uint32_t index;
  if (action == 0 || !runtime_ordinary_ram_reservation_v1_valid(action))
    return RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_INPUT;
  action->status = RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_LIFECYCLE;
  if (!runtime_ordinary_ram_reservation_v1_lifecycle_active) return action->status;
  if (runtime_ordinary_ram_reservation_v1_bytes == 0u) {
    action->status = RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_CONFIGURATION;
    return action->status;
  }
  if (action->kind == RUNTIME_ORDINARY_RAM_RESERVATION_V1_RELEASE) {
    for (index = 0u; index < RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAX_RECORDS; ++index) {
      if (runtime_ordinary_ram_reservation_v1_records[index].opaque_id == action->opaque_id) {
        memset(&runtime_ordinary_ram_reservation_v1_records[index], 0,
          sizeof(runtime_ordinary_ram_reservation_v1_records[index]));
        action->status = RUNTIME_ORDINARY_RAM_RESERVATION_V1_OK;
        return action->status;
      }
    }
    action->status = RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_ID;
    return action->status;
  }
  {
    uint64_t limit = (uint64_t) runtime_ordinary_ram_reservation_v1_base +
      runtime_ordinary_ram_reservation_v1_bytes;
    uint64_t candidate = ((uint64_t) runtime_ordinary_ram_reservation_v1_base +
      action->alignment_bytes - 1u) & ~(uint64_t)(action->alignment_bytes - 1u);
    uint32_t free_index = RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAX_RECORDS;
    for (index = 0u; index < RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAX_RECORDS; ++index)
      if (runtime_ordinary_ram_reservation_v1_records[index].opaque_id == 0u) {
        free_index = index; break;
      }
    if (free_index == RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAX_RECORDS) {
      action->status = RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_CAPACITY;
      return action->status;
    }
    for (;;) {
      int moved = 0;
      if (candidate + action->byte_count > limit) break;
      for (index = 0u; index < RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAX_RECORDS; ++index) {
        const struct runtime_ordinary_ram_reservation_v1_record *record =
          &runtime_ordinary_ram_reservation_v1_records[index];
        if (record->opaque_id != 0u && candidate < (uint64_t)record->address + record->byte_count &&
            (uint64_t)record->address < candidate + action->byte_count) {
          candidate = ((uint64_t)record->address + record->byte_count +
            action->alignment_bytes - 1u) & ~(uint64_t)(action->alignment_bytes - 1u);
          moved = 1; break;
        }
      }
      if (!moved) {
        if (!runtime_ordinary_ram_reservation_v1_span_ordinary((uint32_t)candidate,
            action->byte_count)) {
          action->status = RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_MEMORY;
          return action->status;
        }
        if (runtime_ordinary_ram_reservation_v1_next_id == 0u)
          runtime_ordinary_ram_reservation_v1_next_id = 1u;
        runtime_ordinary_ram_reservation_v1_records[free_index].opaque_id =
          runtime_ordinary_ram_reservation_v1_next_id++;
        runtime_ordinary_ram_reservation_v1_records[free_index].address = (uint32_t)candidate;
        runtime_ordinary_ram_reservation_v1_records[free_index].byte_count = action->byte_count;
        action->opaque_id = runtime_ordinary_ram_reservation_v1_records[free_index].opaque_id;
        action->address = (uint32_t)candidate;
        action->status = RUNTIME_ORDINARY_RAM_RESERVATION_V1_OK;
        return action->status;
      }
    }
  }
  action->status = RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_CAPACITY;
  return action->status;
}
