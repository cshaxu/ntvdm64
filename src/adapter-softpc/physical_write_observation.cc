/////////////////////////////////////////////////////////////////////////
//
// Default-off, selector-blind fixed-range physical-write observation.
//
/////////////////////////////////////////////////////////////////////////

#include "physical_write_observation.h"

#include <string.h>

static uint64_t runtime_physical_write_observation_v1_start;
static uint64_t runtime_physical_write_observation_v1_length;
static struct runtime_physical_write_observation_v1
  runtime_physical_write_observation_v1_value;

static void runtime_physical_write_observation_v1_clear(void)
{
  memset(&runtime_physical_write_observation_v1_value, 0,
    sizeof(runtime_physical_write_observation_v1_value));
  runtime_physical_write_observation_v1_value.magic =
    RUNTIME_PHYSICAL_WRITE_OBSERVATION_V1_MAGIC;
  runtime_physical_write_observation_v1_value.abi_version =
    RUNTIME_PHYSICAL_WRITE_OBSERVATION_V1_VERSION;
  runtime_physical_write_observation_v1_value.struct_bytes =
    sizeof(runtime_physical_write_observation_v1_value);
}

static int runtime_physical_write_observation_v1_overlaps(uint64_t address,
  uint64_t length)
{
  uint64_t start = runtime_physical_write_observation_v1_start;
  uint64_t watch_length = runtime_physical_write_observation_v1_length;
  if (length == 0u || watch_length == 0u || address > UINT64_MAX - length ||
      start > UINT64_MAX - watch_length) return 0;
  return address < start + watch_length && start < address + length;
}

extern "C" int runtime_physical_write_observation_v1_configure(
  uint64_t physical_address, uint64_t byte_count)
{
  if (byte_count == 0u || physical_address > UINT64_MAX - byte_count) return 0;
  runtime_physical_write_observation_v1_start = physical_address;
  runtime_physical_write_observation_v1_length = byte_count;
  runtime_physical_write_observation_v1_clear();
  return 1;
}

extern "C" void runtime_physical_write_observation_v1_reset(void)
{
  runtime_physical_write_observation_v1_start = 0u;
  runtime_physical_write_observation_v1_length = 0u;
  runtime_physical_write_observation_v1_clear();
}

extern "C" int runtime_physical_write_observation_v1_copy(
  struct runtime_physical_write_observation_v1 *value)
{
  if (value == 0 || !runtime_physical_write_observation_v1_value.valid)
    return 0;
  *value = runtime_physical_write_observation_v1_value;
  return 1;
}

extern "C" void runtime_physical_write_observation_v1_record(
  uint64_t physical_address, uint32_t byte_count, const void *bytes,
  uint64_t sequence, uint64_t rip, uint64_t cs_base, uint16_t cs, uint16_t ss,
  uint16_t sp)
{
  struct runtime_physical_write_observation_v1 *value =
    &runtime_physical_write_observation_v1_value;
  uint32_t capture;
  if (value->valid || bytes == 0 || !runtime_physical_write_observation_v1_overlaps(
      physical_address, byte_count)) return;
  capture = byte_count < RUNTIME_PHYSICAL_WRITE_OBSERVATION_V1_BYTES ?
    byte_count : RUNTIME_PHYSICAL_WRITE_OBSERVATION_V1_BYTES;
  value->physical_address = physical_address;
  value->byte_count = byte_count;
  value->captured_bytes = capture;
  value->sequence = sequence;
  value->rip = rip;
  value->cs_base = cs_base;
  value->cs = cs;
  value->ss = ss;
  value->sp = sp;
  memcpy(value->bytes, bytes, capture);
  value->valid = 1u;
}
