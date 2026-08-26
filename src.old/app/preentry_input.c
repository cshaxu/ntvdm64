#include "preentry_input.h"

#include <string.h>

void runtime_preentry_input_clear(struct runtime_preentry_input *input)
{
  if (input == 0) return;
  memset(input, 0, sizeof(*input));
  input->magic = RUNTIME_PREENTRY_INPUT_MAGIC;
  input->abi_version = RUNTIME_PREENTRY_INPUT_VERSION;
  input->struct_bytes = sizeof(*input);
}

int runtime_preentry_input_valid(const struct runtime_preentry_input *input)
{
  uint32_t index, other;
  uint64_t total = 0u;
  if (input == 0 || input->magic != RUNTIME_PREENTRY_INPUT_MAGIC ||
      input->abi_version != RUNTIME_PREENTRY_INPUT_VERSION ||
      input->struct_bytes != sizeof(*input) || input->reserved0 != 0u ||
      input->range_count == 0u ||
      input->range_count > RUNTIME_PREENTRY_INPUT_MAX_RANGES ||
      input->payload_bytes == 0u ||
      input->payload_bytes > RUNTIME_PREENTRY_INPUT_MAX_BYTES) return 0;
  for (index = 0u; index < input->range_count; ++index) {
    const struct runtime_preentry_input_range *range = &input->ranges[index];
    uint64_t payload_end = (uint64_t)range->payload_offset + range->byte_count;
    if (range->byte_count == 0u || payload_end > input->payload_bytes ||
        range->physical_address > UINT64_MAX - range->byte_count ||
        total > UINT64_MAX - range->byte_count) return 0;
    for (other = 0u; other < index; ++other) {
      const struct runtime_preentry_input_range *prior = &input->ranges[other];
      if (range->physical_address < prior->physical_address + prior->byte_count &&
          prior->physical_address < range->physical_address + range->byte_count)
        return 0;
      if (range->payload_offset < prior->payload_offset + prior->byte_count &&
          prior->payload_offset < range->payload_offset + range->byte_count)
        return 0;
    }
    total += range->byte_count;
  }
  return total == input->payload_bytes;
}

int runtime_preentry_input_make_write_action(
  const struct runtime_preentry_input *input,
  struct runtime_mechanical_action *action)
{
  uint32_t index;
  if (!runtime_preentry_input_valid(input) || action == 0) return 0;
  runtime_mechanical_action_clear(action);
  action->action_id = 1u;
  action->kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  action->range_count = input->range_count;
  action->payload_bytes = input->payload_bytes;
  for (index = 0u; index < input->range_count; ++index) {
    action->ranges[index].physical_address = input->ranges[index].physical_address;
    action->ranges[index].byte_count = input->ranges[index].byte_count;
    action->ranges[index].payload_offset = input->ranges[index].payload_offset;
  }
  memcpy(action->payload, input->payload, input->payload_bytes);
  return runtime_mechanical_action_valid(action);
}
