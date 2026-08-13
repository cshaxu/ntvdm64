#ifndef BX_NTVDM_PREENTRY_INPUT_V1_H
#define BX_NTVDM_PREENTRY_INPUT_V1_H

#include <stdint.h>

#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

#define BX_NTVDM_PREENTRY_INPUT_V1_MAGIC 0x42585049u
#define BX_NTVDM_PREENTRY_INPUT_V1_VERSION 1u
#define BX_NTVDM_PREENTRY_INPUT_V1_MAX_RANGES 4u
#define BX_NTVDM_PREENTRY_INPUT_V1_MAX_BYTES 64u

struct bx_ntvdm_preentry_input_v1_range {
  uint64_t physical_address;
  uint32_t byte_count;
  uint32_t payload_offset;
};

struct bx_ntvdm_preentry_input_v1 {
  uint32_t magic, abi_version, struct_bytes, range_count;
  uint32_t payload_bytes, reserved0;
  struct bx_ntvdm_preentry_input_v1_range
    ranges[BX_NTVDM_PREENTRY_INPUT_V1_MAX_RANGES];
  uint8_t payload[BX_NTVDM_PREENTRY_INPUT_V1_MAX_BYTES];
};

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_preentry_input_v1_clear(struct bx_ntvdm_preentry_input_v1 *input);
int bx_ntvdm_preentry_input_v1_valid(const struct bx_ntvdm_preentry_input_v1 *input);
int bx_ntvdm_preentry_input_v1_make_write_action(
  const struct bx_ntvdm_preentry_input_v1 *input,
  struct bx_ntvdm_mechanical_action_v1 *action);

#ifdef __cplusplus
}
#endif

#endif
