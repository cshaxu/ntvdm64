#include "bx_ntvdm_interrupt_return_observation_v1.h"

#include <string.h>

static struct bx_ntvdm_interrupt_return_observation_v1_record
  bx_ntvdm_interrupt_return_observation_records[
    BX_NTVDM_INTERRUPT_RETURN_OBSERVATION_V1_CAPACITY_MAX];
static uint32_t bx_ntvdm_interrupt_return_observation_capacity;
static uint32_t bx_ntvdm_interrupt_return_observation_count_value;
static uint32_t bx_ntvdm_interrupt_return_observation_next;

void bx_ntvdm_mantle_interrupt_return_observation_v1_reset(void)
{
  memset(bx_ntvdm_interrupt_return_observation_records, 0,
    sizeof(bx_ntvdm_interrupt_return_observation_records));
  bx_ntvdm_interrupt_return_observation_count_value = 0u;
  bx_ntvdm_interrupt_return_observation_next = 0u;
}

int bx_ntvdm_mantle_interrupt_return_observation_v1_configure(uint32_t capacity)
{
  if (capacity > BX_NTVDM_INTERRUPT_RETURN_OBSERVATION_V1_CAPACITY_MAX)
    return 0;
  bx_ntvdm_interrupt_return_observation_capacity = capacity;
  bx_ntvdm_mantle_interrupt_return_observation_v1_reset();
  return 1;
}

void bx_ntvdm_mantle_interrupt_return_observation_v1_record(
  const struct bx_ntvdm_interrupt_return_observation_v1_record *record)
{
  if (!record ||
      record->version != BX_NTVDM_INTERRUPT_RETURN_OBSERVATION_V1_VERSION ||
      bx_ntvdm_interrupt_return_observation_capacity == 0u) return;
  bx_ntvdm_interrupt_return_observation_records[
    bx_ntvdm_interrupt_return_observation_next] = *record;
  bx_ntvdm_interrupt_return_observation_next =
    (bx_ntvdm_interrupt_return_observation_next + 1u) %
    bx_ntvdm_interrupt_return_observation_capacity;
  if (bx_ntvdm_interrupt_return_observation_count_value <
      bx_ntvdm_interrupt_return_observation_capacity)
    bx_ntvdm_interrupt_return_observation_count_value++;
}

uint32_t bx_ntvdm_mantle_interrupt_return_observation_v1_count(void)
{
  return bx_ntvdm_interrupt_return_observation_count_value;
}

int bx_ntvdm_mantle_interrupt_return_observation_v1_get(uint32_t index,
  struct bx_ntvdm_interrupt_return_observation_v1_record *record)
{
  uint32_t oldest;
  uint32_t slot;
  if (!record || index >= bx_ntvdm_interrupt_return_observation_count_value ||
      bx_ntvdm_interrupt_return_observation_capacity == 0u) return 0;
  oldest = bx_ntvdm_interrupt_return_observation_count_value ==
    bx_ntvdm_interrupt_return_observation_capacity ?
    bx_ntvdm_interrupt_return_observation_next : 0u;
  slot = (oldest + index) % bx_ntvdm_interrupt_return_observation_capacity;
  *record = bx_ntvdm_interrupt_return_observation_records[slot];
  return 1;
}