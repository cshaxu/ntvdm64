/////////////////////////////////////////////////////////////////////////
//
// Default-off, selector-blind fixed-range physical-write observation.
//
/////////////////////////////////////////////////////////////////////////

#include "bx_ntvdm_physical_write_observation_v1.h"

#include <string.h>

static uint64_t bx_ntvdm_physical_write_observation_v1_start;
static uint64_t bx_ntvdm_physical_write_observation_v1_length;
static struct bx_ntvdm_physical_write_observation_v1
  bx_ntvdm_physical_write_observation_v1_value;

static void bx_ntvdm_physical_write_observation_v1_clear(void)
{
  memset(&bx_ntvdm_physical_write_observation_v1_value, 0,
    sizeof(bx_ntvdm_physical_write_observation_v1_value));
  bx_ntvdm_physical_write_observation_v1_value.magic =
    BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_MAGIC;
  bx_ntvdm_physical_write_observation_v1_value.abi_version =
    BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_VERSION;
  bx_ntvdm_physical_write_observation_v1_value.struct_bytes =
    sizeof(bx_ntvdm_physical_write_observation_v1_value);
}

static int bx_ntvdm_physical_write_observation_v1_overlaps(uint64_t address,
  uint64_t length)
{
  uint64_t start = bx_ntvdm_physical_write_observation_v1_start;
  uint64_t watch_length = bx_ntvdm_physical_write_observation_v1_length;
  if (length == 0u || watch_length == 0u || address > UINT64_MAX - length ||
      start > UINT64_MAX - watch_length) return 0;
  return address < start + watch_length && start < address + length;
}

extern "C" int bx_ntvdm_physical_write_observation_v1_configure(
  uint64_t physical_address, uint64_t byte_count)
{
  if (byte_count == 0u || physical_address > UINT64_MAX - byte_count) return 0;
  bx_ntvdm_physical_write_observation_v1_start = physical_address;
  bx_ntvdm_physical_write_observation_v1_length = byte_count;
  bx_ntvdm_physical_write_observation_v1_clear();
  return 1;
}

extern "C" void bx_ntvdm_physical_write_observation_v1_reset(void)
{
  bx_ntvdm_physical_write_observation_v1_start = 0u;
  bx_ntvdm_physical_write_observation_v1_length = 0u;
  bx_ntvdm_physical_write_observation_v1_clear();
}

extern "C" int bx_ntvdm_physical_write_observation_v1_copy(
  struct bx_ntvdm_physical_write_observation_v1 *value)
{
  if (value == 0 || !bx_ntvdm_physical_write_observation_v1_value.valid)
    return 0;
  *value = bx_ntvdm_physical_write_observation_v1_value;
  return 1;
}

extern "C" void bx_ntvdm_physical_write_observation_v1_record(
  uint64_t physical_address, uint32_t byte_count, const void *bytes,
  uint64_t sequence, uint64_t rip, uint64_t cs_base, uint16_t cs, uint16_t ss,
  uint16_t sp)
{
  struct bx_ntvdm_physical_write_observation_v1 *value =
    &bx_ntvdm_physical_write_observation_v1_value;
  uint32_t capture;
  if (value->valid || bytes == 0 || !bx_ntvdm_physical_write_observation_v1_overlaps(
      physical_address, byte_count)) return;
  capture = byte_count < BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_BYTES ?
    byte_count : BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_BYTES;
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
