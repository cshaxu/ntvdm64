#include "software_interrupt_observation.h"

#include <string.h>

static struct runtime_software_interrupt_observation_record
  runtime_software_interrupt_observation_records[
    RUNTIME_SOFTWARE_INTERRUPT_OBSERVATION_CAPACITY_MAX];
static uint32_t runtime_software_interrupt_observation_capacity;
static uint32_t runtime_software_interrupt_observation_count_value;
static uint32_t runtime_software_interrupt_observation_next;

void runtime_machine_software_interrupt_observation_reset(void)
{
  memset(runtime_software_interrupt_observation_records, 0,
    sizeof(runtime_software_interrupt_observation_records));
  runtime_software_interrupt_observation_count_value = 0u;
  runtime_software_interrupt_observation_next = 0u;
}

int runtime_machine_software_interrupt_observation_configure(uint32_t capacity)
{
  if (capacity > RUNTIME_SOFTWARE_INTERRUPT_OBSERVATION_CAPACITY_MAX)
    return 0;
  runtime_software_interrupt_observation_capacity = capacity;
  runtime_machine_software_interrupt_observation_reset();
  return 1;
}

void runtime_machine_software_interrupt_observation_record(
  const struct runtime_software_interrupt_observation_record *record)
{
  if (!record || record->version != RUNTIME_SOFTWARE_INTERRUPT_OBSERVATION_VERSION ||
      runtime_software_interrupt_observation_capacity == 0u) return;
  runtime_software_interrupt_observation_records[
    runtime_software_interrupt_observation_next] = *record;
  runtime_software_interrupt_observation_next =
    (runtime_software_interrupt_observation_next + 1u) %
    runtime_software_interrupt_observation_capacity;
  if (runtime_software_interrupt_observation_count_value <
      runtime_software_interrupt_observation_capacity)
    runtime_software_interrupt_observation_count_value++;
}

uint32_t runtime_machine_software_interrupt_observation_count(void)
{
  return runtime_software_interrupt_observation_count_value;
}

int runtime_machine_software_interrupt_observation_get(uint32_t index,
  struct runtime_software_interrupt_observation_record *record)
{
  uint32_t oldest;
  uint32_t slot;
  if (!record || index >= runtime_software_interrupt_observation_count_value ||
      runtime_software_interrupt_observation_capacity == 0u) return 0;
  oldest = runtime_software_interrupt_observation_count_value ==
    runtime_software_interrupt_observation_capacity ?
    runtime_software_interrupt_observation_next : 0u;
  slot = (oldest + index) % runtime_software_interrupt_observation_capacity;
  *record = runtime_software_interrupt_observation_records[slot];
  return 1;
}
