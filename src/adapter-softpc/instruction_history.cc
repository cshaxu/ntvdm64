/////////////////////////////////////////////////////////////////////////
//
// Mantle-owned bounded storage for copied native instruction positions.
//
/////////////////////////////////////////////////////////////////////////

#include "instruction_history.h"
#include "machine_binding.h"
#include <string.h>

#ifndef RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
#define RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE 0
#endif

#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
#endif

static struct runtime_instruction_history_record_v1
  runtime_instruction_history_records[RUNTIME_INSTRUCTION_HISTORY_V1_CAPACITY_MAX];
static uint32_t runtime_instruction_history_capacity;
static uint32_t runtime_instruction_history_count_value;
static uint32_t runtime_instruction_history_next;
static struct runtime_instruction_history_record_v1
  runtime_instruction_history_last_record;
static unsigned runtime_instruction_history_last_record_valid;
static struct runtime_instruction_history_transition_v1
  runtime_instruction_history_latest_cs_transition;
static unsigned runtime_instruction_history_latest_cs_transition_valid;
static struct runtime_instruction_history_transition_v1
  runtime_instruction_history_cs_transitions[
    RUNTIME_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX];
static uint32_t runtime_instruction_history_cs_transition_count_value;
static uint32_t runtime_instruction_history_cs_transition_next;
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
static struct runtime_instruction_history_provenance_v1
  runtime_instruction_history_latest_cs_provenance;
static unsigned runtime_instruction_history_latest_cs_provenance_valid;

static int runtime_instruction_history_real_address(uint16_t segment,
  uint16_t offset, uint32_t byte_count, uint64_t *address)
{
  uint32_t value;
  if (!address || byte_count == 0u) return 0;
  value = ((uint32_t)segment << 4) + offset;
  if (value > 0x100000u - byte_count) return 0;
  *address = value;
  return 1;
}
#endif

void runtime_mantle_instruction_history_v1_reset(void)
{
  memset(runtime_instruction_history_records, 0,
    sizeof(runtime_instruction_history_records));
  runtime_instruction_history_count_value = 0;
  runtime_instruction_history_next = 0;
  memset(&runtime_instruction_history_last_record, 0,
    sizeof(runtime_instruction_history_last_record));
  runtime_instruction_history_last_record_valid = 0;
  memset(&runtime_instruction_history_latest_cs_transition, 0,
    sizeof(runtime_instruction_history_latest_cs_transition));
  runtime_instruction_history_latest_cs_transition_valid = 0;
  memset(runtime_instruction_history_cs_transitions, 0,
    sizeof(runtime_instruction_history_cs_transitions));
  runtime_instruction_history_cs_transition_count_value = 0;
  runtime_instruction_history_cs_transition_next = 0;
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  memset(&runtime_instruction_history_latest_cs_provenance, 0,
    sizeof(runtime_instruction_history_latest_cs_provenance));
  runtime_instruction_history_latest_cs_provenance_valid = 0;
#endif
}

int runtime_mantle_instruction_history_v1_configure(uint32_t capacity)
{
  if (capacity > RUNTIME_INSTRUCTION_HISTORY_V1_CAPACITY_MAX) return 0;
  runtime_instruction_history_capacity = capacity;
  runtime_mantle_instruction_history_v1_reset();
  return 1;
}

void runtime_mantle_instruction_history_v1_record(
  const struct runtime_instruction_history_record_v1 *record)
{
  if (!record || record->version != RUNTIME_INSTRUCTION_HISTORY_V1_VERSION ||
      runtime_instruction_history_capacity == 0) return;
  if (runtime_instruction_history_last_record_valid &&
      runtime_instruction_history_last_record.cs != record->cs) {
    runtime_instruction_history_latest_cs_transition.previous =
      runtime_instruction_history_last_record;
    runtime_instruction_history_latest_cs_transition.current = *record;
    runtime_instruction_history_latest_cs_transition_valid = 1;
    runtime_instruction_history_cs_transitions[
      runtime_instruction_history_cs_transition_next] =
      runtime_instruction_history_latest_cs_transition;
    runtime_instruction_history_cs_transition_next =
      (runtime_instruction_history_cs_transition_next + 1u) %
      RUNTIME_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX;
    if (runtime_instruction_history_cs_transition_count_value <
        RUNTIME_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX)
      runtime_instruction_history_cs_transition_count_value++;
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
    uint64_t predecessor_address, successor_address, stack_address;
    memset(&runtime_instruction_history_latest_cs_provenance, 0,
      sizeof(runtime_instruction_history_latest_cs_provenance));
    runtime_instruction_history_latest_cs_provenance.transition =
      runtime_instruction_history_latest_cs_transition;
    if (runtime_instruction_history_real_address(
        runtime_instruction_history_last_record.cs,
        (uint16_t)runtime_instruction_history_last_record.rip,
        RUNTIME_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES,
        &predecessor_address) && runtime_machine_binding_v1_memory_read(
          predecessor_address, RUNTIME_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES,
          runtime_instruction_history_latest_cs_provenance.predecessor_bytes)) {
      runtime_instruction_history_latest_cs_provenance.predecessor_valid = 1u;
    }
    if (runtime_instruction_history_real_address(record->cs,
        (uint16_t)record->rip,
        RUNTIME_INSTRUCTION_HISTORY_V1_SUCCESSOR_BYTES,
        &successor_address) && runtime_machine_binding_v1_memory_read(
          successor_address, RUNTIME_INSTRUCTION_HISTORY_V1_SUCCESSOR_BYTES,
          runtime_instruction_history_latest_cs_provenance.successor_bytes)) {
      runtime_instruction_history_latest_cs_provenance.successor_valid = 1u;
    }
    if (runtime_instruction_history_real_address(record->ss, record->sp,
        RUNTIME_INSTRUCTION_HISTORY_V1_STACK_BYTES, &stack_address) &&
        runtime_machine_binding_v1_memory_read(stack_address,
          RUNTIME_INSTRUCTION_HISTORY_V1_STACK_BYTES,
          runtime_instruction_history_latest_cs_provenance.stack_bytes)) {
      runtime_instruction_history_latest_cs_provenance.stack_valid = 1u;
    }
    runtime_instruction_history_latest_cs_provenance_valid = 1;
#endif
  }
  runtime_instruction_history_last_record = *record;
  runtime_instruction_history_last_record_valid = 1;
  runtime_instruction_history_records[runtime_instruction_history_next] = *record;
  runtime_instruction_history_next =
    (runtime_instruction_history_next + 1) % runtime_instruction_history_capacity;
  if (runtime_instruction_history_count_value < runtime_instruction_history_capacity) {
    runtime_instruction_history_count_value++;
  }
}

uint32_t runtime_mantle_instruction_history_v1_count(void)
{
  return runtime_instruction_history_count_value;
}

int runtime_mantle_instruction_history_v1_get(uint32_t index,
  struct runtime_instruction_history_record_v1 *record)
{
  uint32_t oldest;
  uint32_t slot;
  if (!record || index >= runtime_instruction_history_count_value ||
      runtime_instruction_history_capacity == 0) return 0;
  oldest = runtime_instruction_history_count_value == runtime_instruction_history_capacity ?
    runtime_instruction_history_next : 0;
  slot = (oldest + index) % runtime_instruction_history_capacity;
  *record = runtime_instruction_history_records[slot];
  return 1;
}

int runtime_mantle_instruction_history_v1_get_latest_cs_transition(
  struct runtime_instruction_history_transition_v1 *transition)
{
  if (!transition || !runtime_instruction_history_latest_cs_transition_valid)
    return 0;
  *transition = runtime_instruction_history_latest_cs_transition;
  return 1;
}

uint32_t runtime_mantle_instruction_history_v1_cs_transition_count(void)
{
  return runtime_instruction_history_cs_transition_count_value;
}

int runtime_mantle_instruction_history_v1_get_cs_transition(uint32_t index,
  struct runtime_instruction_history_transition_v1 *transition)
{
  uint32_t oldest;
  uint32_t slot;
  if (!transition || index >= runtime_instruction_history_cs_transition_count_value)
    return 0;
  oldest = runtime_instruction_history_cs_transition_count_value ==
    RUNTIME_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX ?
    runtime_instruction_history_cs_transition_next : 0u;
  slot = (oldest + index) %
    RUNTIME_INSTRUCTION_HISTORY_V1_CS_TRANSITION_CAPACITY_MAX;
  *transition = runtime_instruction_history_cs_transitions[slot];
  return 1;
}

int runtime_mantle_instruction_history_v1_get_latest_cs_provenance(
  struct runtime_instruction_history_provenance_v1 *provenance)
{
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
  if (!provenance || !runtime_instruction_history_latest_cs_provenance_valid)
    return 0;
  *provenance = runtime_instruction_history_latest_cs_provenance;
  return 1;
#else
  (void)provenance;
  return 0;
#endif
}
