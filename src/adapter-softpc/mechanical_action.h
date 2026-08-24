/////////////////////////////////////////////////////////////////////////
//
// Fixed synchronous ordinary-RAM action owned by the native Bochs mantle.
// It carries only physical ranges and copied bytes: no higher-layer meaning
// is represented here.
//
/////////////////////////////////////////////////////////////////////////

#ifndef RUNTIME_MECHANICAL_ACTION_V1_H
#define RUNTIME_MECHANICAL_ACTION_V1_H

#include <stdint.h>
#include <string.h>

#define RUNTIME_MECHANICAL_ACTION_V1_MAGIC 0x42584d41u
#define RUNTIME_MECHANICAL_ACTION_V1_VERSION 1u
#define RUNTIME_MECHANICAL_ACTION_V1_MAX_RANGES 64u
#define RUNTIME_MECHANICAL_ACTION_V1_MAX_BYTES 65535u

enum runtime_mechanical_action_v1_kind {
  RUNTIME_MECHANICAL_ACTION_V1_NONE = 0u,
  RUNTIME_MECHANICAL_ACTION_V1_READ = 1u,
  RUNTIME_MECHANICAL_ACTION_V1_WRITE = 2u,
  RUNTIME_MECHANICAL_ACTION_V1_PREFLIGHT_WRITE = 3u
};

struct runtime_mechanical_action_v1_range {
  uint64_t physical_address;
  uint32_t byte_count;
  uint32_t payload_offset;
};

struct runtime_mechanical_action_v1 {
  uint32_t magic, abi_version, struct_bytes, kind;
  uint32_t action_id, range_count;
  uint32_t payload_bytes, reserved0;
  struct runtime_mechanical_action_v1_range
    ranges[RUNTIME_MECHANICAL_ACTION_V1_MAX_RANGES];
  uint8_t payload[RUNTIME_MECHANICAL_ACTION_V1_MAX_BYTES];
};

#ifdef __cplusplus
extern "C" {
#endif

/* Records are borrowed only for this call.  A successful read copies bytes
 * into payload; a successful write consumes its copied payload. */
static inline void runtime_mechanical_action_v1_clear(
  struct runtime_mechanical_action_v1 *action)
{
  if (action == 0) return;
  memset(action, 0, sizeof(*action));
  action->magic = RUNTIME_MECHANICAL_ACTION_V1_MAGIC;
  action->abi_version = RUNTIME_MECHANICAL_ACTION_V1_VERSION;
  action->struct_bytes = sizeof(*action);
}
static inline int runtime_mechanical_action_v1_valid(
  const struct runtime_mechanical_action_v1 *action)
{
  uint32_t index; uint64_t total = 0;
  if (action == 0 || action->magic != RUNTIME_MECHANICAL_ACTION_V1_MAGIC ||
      action->abi_version != RUNTIME_MECHANICAL_ACTION_V1_VERSION ||
      action->struct_bytes != sizeof(*action) || action->action_id == 0 ||
      ((action->kind != RUNTIME_MECHANICAL_ACTION_V1_READ &&
       action->kind != RUNTIME_MECHANICAL_ACTION_V1_WRITE &&
       action->kind != RUNTIME_MECHANICAL_ACTION_V1_PREFLIGHT_WRITE)) ||
      action->range_count == 0 ||
      action->range_count > RUNTIME_MECHANICAL_ACTION_V1_MAX_RANGES ||
      action->payload_bytes == 0 ||
      action->payload_bytes > RUNTIME_MECHANICAL_ACTION_V1_MAX_BYTES) return 0;
  for (index = 0; index < action->range_count; ++index) {
    const struct runtime_mechanical_action_v1_range *range = &action->ranges[index];
    uint64_t payload_end = (uint64_t)range->payload_offset + range->byte_count;
    if (range->byte_count == 0 || payload_end > action->payload_bytes ||
        range->physical_address > UINT64_MAX - range->byte_count ||
        total > UINT64_MAX - range->byte_count) return 0;
    total += range->byte_count;
  }
  return total == action->payload_bytes;
}
int runtime_mantle_execute_mechanical_action_v1(
  struct runtime_mechanical_action_v1 *action);

/* Synchronous checked ordinary-RAM transport for an active machine stage.
 * This is deliberately selector-blind: callers provide only an address and
 * copied bytes, and no guest/service meaning crosses this mantle boundary. */
int runtime_mantle_checked_ram_read_v1(uint64_t physical_address,
  uint8_t *bytes, uint32_t byte_count);
int runtime_mantle_checked_ram_write_v1(uint64_t physical_address,
  const uint8_t *bytes, uint32_t byte_count);

#ifdef __cplusplus
}
#endif

#endif
