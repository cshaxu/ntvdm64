#ifndef RUNTIME_PREENTRY_INPUT_H
#define RUNTIME_PREENTRY_INPUT_H

#include <stdint.h>

#include "adapter-softpc/mechanical_action.h"

#define RUNTIME_PREENTRY_INPUT_MAGIC 0x42585049u
#define RUNTIME_PREENTRY_INPUT_VERSION 1u
#define RUNTIME_PREENTRY_INPUT_MAX_RANGES 4u
#define RUNTIME_PREENTRY_INPUT_MAX_BYTES 64u

struct runtime_preentry_input_range {
  uint64_t physical_address;
  uint32_t byte_count;
  uint32_t payload_offset;
};

struct runtime_preentry_input {
  uint32_t magic, abi_version, struct_bytes, range_count;
  uint32_t payload_bytes, reserved0;
  struct runtime_preentry_input_range
    ranges[RUNTIME_PREENTRY_INPUT_MAX_RANGES];
  uint8_t payload[RUNTIME_PREENTRY_INPUT_MAX_BYTES];
};

#ifdef __cplusplus
extern "C" {
#endif

void runtime_preentry_input_clear(struct runtime_preentry_input *input);
int runtime_preentry_input_valid(const struct runtime_preentry_input *input);
int runtime_preentry_input_make_write_action(
  const struct runtime_preentry_input *input,
  struct runtime_mechanical_action *action);

#ifdef __cplusplus
}
#endif

#endif
