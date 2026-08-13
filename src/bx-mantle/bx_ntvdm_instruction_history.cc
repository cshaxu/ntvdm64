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
static struct bx_ntvdm_instruction_history_record_v1
  bx_ntvdm_instruction_history_last_record;
static unsigned bx_ntvdm_instruction_history_last_record_valid;
static struct bx_ntvdm_instruction_history_transition_v1
  bx_ntvdm_instruction_history_latest_cs_transition;
static unsigned bx_ntvdm_instruction_history_latest_cs_transition_valid;

void bx_ntvdm_mantle_instruction_history_v1_reset(void)
{
  memset(bx_ntvdm_instruction_history_records, 0,
    sizeof(bx_ntvdm_instruction_history_records));
  bx_ntvdm_instruction_history_count_value = 0;
  bx_ntvdm_instruction_history_next = 0;
  memset(&bx_ntvdm_instruction_history_last_record, 0,
    sizeof(bx_ntvdm_instruction_history_last_record));
  bx_ntvdm_instruction_history_last_record_valid = 0;
  memset(&bx_ntvdm_instruction_history_latest_cs_transition, 0,
    sizeof(bx_ntvdm_instruction_history_latest_cs_transition));
  bx_ntvdm_instruction_history_latest_cs_transition_valid = 0;
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
  if (bx_ntvdm_instruction_history_last_record_valid &&
      bx_ntvdm_instruction_history_last_record.cs != record->cs) {
    bx_ntvdm_instruction_history_latest_cs_transition.previous =
      bx_ntvdm_instruction_history_last_record;
    bx_ntvdm_instruction_history_latest_cs_transition.current = *record;
    bx_ntvdm_instruction_history_latest_cs_transition_valid = 1;
  }
  bx_ntvdm_instruction_history_last_record = *record;
  bx_ntvdm_instruction_history_last_record_valid = 1;
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

int bx_ntvdm_mantle_instruction_history_v1_get_latest_cs_transition(
  struct bx_ntvdm_instruction_history_transition_v1 *transition)
{
  if (!transition || !bx_ntvdm_instruction_history_latest_cs_transition_valid)
    return 0;
  *transition = bx_ntvdm_instruction_history_latest_cs_transition;
  return 1;
}
