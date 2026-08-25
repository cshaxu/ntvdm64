#include "interrupt_return_observation.h"

#include <string.h>

static struct runtime_interrupt_return_observation_record
  runtime_interrupt_return_observation_records[
    RUNTIME_INTERRUPT_RETURN_OBSERVATION_CAPACITY_MAX];
static uint32_t runtime_interrupt_return_observation_capacity;
static uint32_t runtime_interrupt_return_observation_count_value;
static uint32_t runtime_interrupt_return_observation_next;

void runtime_machine_interrupt_return_observation_reset(void)
{
  memset(runtime_interrupt_return_observation_records, 0,
    sizeof(runtime_interrupt_return_observation_records));
  runtime_interrupt_return_observation_count_value = 0u;
  runtime_interrupt_return_observation_next = 0u;
}

int runtime_machine_interrupt_return_observation_configure(uint32_t capacity)
{
  if (capacity > RUNTIME_INTERRUPT_RETURN_OBSERVATION_CAPACITY_MAX)
    return 0;
  runtime_interrupt_return_observation_capacity = capacity;
  runtime_machine_interrupt_return_observation_reset();
  return 1;
}

void runtime_machine_interrupt_return_observation_record(
  const struct runtime_interrupt_return_observation_record *record)
{
  if (!record ||
      record->version != RUNTIME_INTERRUPT_RETURN_OBSERVATION_VERSION ||
      runtime_interrupt_return_observation_capacity == 0u) return;
  runtime_interrupt_return_observation_records[
    runtime_interrupt_return_observation_next] = *record;
  runtime_interrupt_return_observation_next =
    (runtime_interrupt_return_observation_next + 1u) %
    runtime_interrupt_return_observation_capacity;
  if (runtime_interrupt_return_observation_count_value <
      runtime_interrupt_return_observation_capacity)
    runtime_interrupt_return_observation_count_value++;
}

uint32_t runtime_machine_interrupt_return_observation_count(void)
{
  return runtime_interrupt_return_observation_count_value;
}

int runtime_machine_interrupt_return_observation_get(uint32_t index,
  struct runtime_interrupt_return_observation_record *record)
{
  uint32_t oldest;
  uint32_t slot;
  if (!record || index >= runtime_interrupt_return_observation_count_value ||
      runtime_interrupt_return_observation_capacity == 0u) return 0;
  oldest = runtime_interrupt_return_observation_count_value ==
    runtime_interrupt_return_observation_capacity ?
    runtime_interrupt_return_observation_next : 0u;
  slot = (oldest + index) % runtime_interrupt_return_observation_capacity;
  *record = runtime_interrupt_return_observation_records[slot];
  return 1;
}