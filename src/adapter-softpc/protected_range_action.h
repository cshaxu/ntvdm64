#ifndef RUNTIME_PROTECTED_RANGE_ACTION_V1_H
#define RUNTIME_PROTECTED_RANGE_ACTION_V1_H

#include <stdint.h>

#define RUNTIME_PROTECTED_RANGE_ACTION_V1_MAGIC 0x42585052u
#define RUNTIME_PROTECTED_RANGE_ACTION_V1_VERSION 1u
#define RUNTIME_PROTECTED_RANGE_ACTION_V1_MAX_BYTES 4096u

enum runtime_protected_range_action_v1_kind {
  RUNTIME_PROTECTED_RANGE_ACTION_V1_READ = 1u,
  RUNTIME_PROTECTED_RANGE_ACTION_V1_WRITE = 2u
};

enum runtime_protected_range_action_v1_status {
  RUNTIME_PROTECTED_RANGE_ACTION_V1_OK = 0u,
  RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_INPUT,
  RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_LIFECYCLE,
  RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MODE,
  RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_ACCESS,
  RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_MEMORY
};

struct runtime_protected_range_action_v1 {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t kind;
  uint32_t segment;
  uint32_t offset;
  uint32_t byte_count;
  uint32_t status;
  uint8_t bytes[RUNTIME_PROTECTED_RANGE_ACTION_V1_MAX_BYTES];
};

#ifdef __cplusplus
extern "C" {
#endif

void runtime_protected_range_action_v1_clear(
  struct runtime_protected_range_action_v1 *action);
int runtime_protected_range_action_v1_valid(
  const struct runtime_protected_range_action_v1 *action);
void runtime_protected_range_action_v1_set_lifecycle_active(uint32_t active);
uint32_t runtime_mantle_execute_protected_range_action_v1(
  struct runtime_protected_range_action_v1 *action);

#ifdef __cplusplus
}
#endif

#endif
