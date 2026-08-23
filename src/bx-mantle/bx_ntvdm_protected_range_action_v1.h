#ifndef BX_NTVDM_PROTECTED_RANGE_ACTION_V1_H
#define BX_NTVDM_PROTECTED_RANGE_ACTION_V1_H

#include <stdint.h>

#define BX_NTVDM_PROTECTED_RANGE_ACTION_V1_MAGIC 0x42585052u
#define BX_NTVDM_PROTECTED_RANGE_ACTION_V1_VERSION 1u
#define BX_NTVDM_PROTECTED_RANGE_ACTION_V1_MAX_BYTES 4096u

enum bx_ntvdm_protected_range_action_v1_kind {
  BX_NTVDM_PROTECTED_RANGE_ACTION_V1_READ = 1u,
  BX_NTVDM_PROTECTED_RANGE_ACTION_V1_WRITE = 2u
};

enum bx_ntvdm_protected_range_action_v1_status {
  BX_NTVDM_PROTECTED_RANGE_ACTION_V1_OK = 0u,
  BX_NTVDM_PROTECTED_RANGE_ACTION_V1_REJECTED_INPUT,
  BX_NTVDM_PROTECTED_RANGE_ACTION_V1_REJECTED_LIFECYCLE,
  BX_NTVDM_PROTECTED_RANGE_ACTION_V1_REJECTED_MODE,
  BX_NTVDM_PROTECTED_RANGE_ACTION_V1_REJECTED_ACCESS,
  BX_NTVDM_PROTECTED_RANGE_ACTION_V1_REJECTED_MEMORY
};

struct bx_ntvdm_protected_range_action_v1 {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t kind;
  uint32_t segment;
  uint32_t offset;
  uint32_t byte_count;
  uint32_t status;
  uint8_t bytes[BX_NTVDM_PROTECTED_RANGE_ACTION_V1_MAX_BYTES];
};

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_protected_range_action_v1_clear(
  struct bx_ntvdm_protected_range_action_v1 *action);
int bx_ntvdm_protected_range_action_v1_valid(
  const struct bx_ntvdm_protected_range_action_v1 *action);
void bx_ntvdm_protected_range_action_v1_set_lifecycle_active(uint32_t active);
uint32_t bx_ntvdm_mantle_execute_protected_range_action_v1(
  struct bx_ntvdm_protected_range_action_v1 *action);

#ifdef __cplusplus
}
#endif

#endif
