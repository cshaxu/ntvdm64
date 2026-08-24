#include "preentry_input_v1.h"

#include <string.h>

void bx_ntvdm_preentry_input_v1_clear(struct bx_ntvdm_preentry_input_v1 *input)
{
  if (input == 0) return;
  memset(input, 0, sizeof(*input));
  input->magic = BX_NTVDM_PREENTRY_INPUT_V1_MAGIC;
  input->abi_version = BX_NTVDM_PREENTRY_INPUT_V1_VERSION;
  input->struct_bytes = sizeof(*input);
}

int bx_ntvdm_preentry_input_v1_valid(const struct bx_ntvdm_preentry_input_v1 *input)
{
  uint32_t index, other;
  uint64_t total = 0u;
  if (input == 0 || input->magic != BX_NTVDM_PREENTRY_INPUT_V1_MAGIC ||
      input->abi_version != BX_NTVDM_PREENTRY_INPUT_V1_VERSION ||
      input->struct_bytes != sizeof(*input) || input->reserved0 != 0u ||
      input->range_count == 0u ||
      input->range_count > BX_NTVDM_PREENTRY_INPUT_V1_MAX_RANGES ||
      input->payload_bytes == 0u ||
      input->payload_bytes > BX_NTVDM_PREENTRY_INPUT_V1_MAX_BYTES) return 0;
  for (index = 0u; index < input->range_count; ++index) {
    const struct bx_ntvdm_preentry_input_v1_range *range = &input->ranges[index];
    uint64_t payload_end = (uint64_t)range->payload_offset + range->byte_count;
    if (range->byte_count == 0u || payload_end > input->payload_bytes ||
        range->physical_address > UINT64_MAX - range->byte_count ||
        total > UINT64_MAX - range->byte_count) return 0;
    for (other = 0u; other < index; ++other) {
      const struct bx_ntvdm_preentry_input_v1_range *prior = &input->ranges[other];
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

int bx_ntvdm_preentry_input_v1_make_write_action(
  const struct bx_ntvdm_preentry_input_v1 *input,
  struct bx_ntvdm_mechanical_action_v1 *action)
{
  uint32_t index;
  if (!bx_ntvdm_preentry_input_v1_valid(input) || action == 0) return 0;
  bx_ntvdm_mechanical_action_v1_clear(action);
  action->action_id = 1u;
  action->kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  action->range_count = input->range_count;
  action->payload_bytes = input->payload_bytes;
  for (index = 0u; index < input->range_count; ++index) {
    action->ranges[index].physical_address = input->ranges[index].physical_address;
    action->ranges[index].byte_count = input->ranges[index].byte_count;
    action->ranges[index].payload_offset = input->ranges[index].payload_offset;
  }
  memcpy(action->payload, input->payload, input->payload_bytes);
  return bx_ntvdm_mechanical_action_v1_valid(action);
}
