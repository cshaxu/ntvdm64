/////////////////////////////////////////////////////////////////////////
//
// Mantle-owned bounded storage for copied native instruction positions.
//
/////////////////////////////////////////////////////////////////////////

#include "bx_ntvdm_instruction_history.h"
#include <string.h>

static struct bx_ntvdm_instruction_history_record_v1
  bx_ntvdm_instruction_history_records[BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX];
static uint32_t bx_ntvdm_instruction_history_capacity;
static uint32_t bx_ntvdm_instruction_history_count_value;
static uint32_t bx_ntvdm_instruction_history_next;

void bx_ntvdm_mantle_instruction_history_v1_reset(void)
{
  memset(bx_ntvdm_instruction_history_records, 0,
    sizeof(bx_ntvdm_instruction_history_records));
  bx_ntvdm_instruction_history_count_value = 0;
  bx_ntvdm_instruction_history_next = 0;
}

int bx_ntvdm_mantle_instruction_history_v1_configure(uint32_t capacity)
{
  if (capacity > BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX) return 0;
  bx_ntvdm_instruction_history_capacity = capacity;
  bx_ntvdm_mantle_instruction_history_v1_reset();
  return 1;
}

void bx_ntvdm_mantle_instruction_history_v1_record(
  const struct bx_ntvdm_instruction_history_record_v1 *record)
{
  if (!record || record->version != BX_NTVDM_INSTRUCTION_HISTORY_V1_VERSION ||
      bx_ntvdm_instruction_history_capacity == 0) return;
  bx_ntvdm_instruction_history_records[bx_ntvdm_instruction_history_next] = *record;
  bx_ntvdm_instruction_history_next =
    (bx_ntvdm_instruction_history_next + 1) % bx_ntvdm_instruction_history_capacity;
  if (bx_ntvdm_instruction_history_count_value < bx_ntvdm_instruction_history_capacity) {
    bx_ntvdm_instruction_history_count_value++;
  }
}

uint32_t bx_ntvdm_mantle_instruction_history_v1_count(void)
{
  return bx_ntvdm_instruction_history_count_value;
}

int bx_ntvdm_mantle_instruction_history_v1_get(uint32_t index,
  struct bx_ntvdm_instruction_history_record_v1 *record)
{
  uint32_t oldest;
  uint32_t slot;
  if (!record || index >= bx_ntvdm_instruction_history_count_value ||
      bx_ntvdm_instruction_history_capacity == 0) return 0;
  oldest = bx_ntvdm_instruction_history_count_value == bx_ntvdm_instruction_history_capacity ?
    bx_ntvdm_instruction_history_next : 0;
  slot = (oldest + index) % bx_ntvdm_instruction_history_capacity;
  *record = bx_ntvdm_instruction_history_records[slot];
  return 1;
}
