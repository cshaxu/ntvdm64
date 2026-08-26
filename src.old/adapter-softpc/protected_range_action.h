#ifndef RUNTIME_PROTECTED_RANGE_ACTION_H
#define RUNTIME_PROTECTED_RANGE_ACTION_H

#include <stdint.h>

#define RUNTIME_PROTECTED_RANGE_ACTION_MAGIC 0x42585052u
#define RUNTIME_PROTECTED_RANGE_ACTION_VERSION 1u
#define RUNTIME_PROTECTED_RANGE_ACTION_MAX_BYTES 4096u

enum runtime_protected_range_action_kind {
  RUNTIME_PROTECTED_RANGE_ACTION_READ = 1u,
  RUNTIME_PROTECTED_RANGE_ACTION_WRITE = 2u
};

enum runtime_protected_range_action_status {
  RUNTIME_PROTECTED_RANGE_ACTION_OK = 0u,
  RUNTIME_PROTECTED_RANGE_ACTION_REJECTED_INPUT,
  RUNTIME_PROTECTED_RANGE_ACTION_REJECTED_LIFECYCLE,
  RUNTIME_PROTECTED_RANGE_ACTION_REJECTED_MODE,
  RUNTIME_PROTECTED_RANGE_ACTION_REJECTED_ACCESS,
  RUNTIME_PROTECTED_RANGE_ACTION_REJECTED_MEMORY
};

struct runtime_protected_range_action {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t kind;
  uint32_t segment;
  uint32_t offset;
  uint32_t byte_count;
  uint32_t status;
  uint8_t bytes[RUNTIME_PROTECTED_RANGE_ACTION_MAX_BYTES];
};

#ifdef __cplusplus
extern "C" {
#endif

void runtime_protected_range_action_clear(
  struct runtime_protected_range_action *action);
int runtime_protected_range_action_valid(
  const struct runtime_protected_range_action *action);
void runtime_protected_range_action_set_lifecycle_active(uint32_t active);
uint32_t runtime_machine_execute_protected_range_action(
  struct runtime_protected_range_action *action);

#ifdef __cplusplus
}
#endif

#endif
