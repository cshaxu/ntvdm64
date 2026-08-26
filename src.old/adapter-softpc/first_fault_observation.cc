/////////////////////////////////////////////////////////////////////////
//
// Default-off first non-UD exception observer.  It holds one copied record
// only for a focused finite-machine diagnostic.
//
/////////////////////////////////////////////////////////////////////////

#include "first_fault_observation.h"

#include <string.h>

static int runtime_first_fault_fixture_enabled;
static int runtime_first_fault_was_observed;
static struct runtime_first_fault_observation runtime_first_fault_event;

extern "C" void runtime_machine_first_fault_observation_reset(void)
{
  runtime_first_fault_was_observed = 0;
  memset(&runtime_first_fault_event, 0, sizeof(runtime_first_fault_event));
}

extern "C" void runtime_machine_first_fault_observation_enable(int enabled)
{
  if (enabled) runtime_machine_first_fault_observation_reset();
  runtime_first_fault_fixture_enabled = enabled != 0;
}

extern "C" int runtime_machine_first_fault_observation_enabled(void)
{
  return runtime_first_fault_fixture_enabled;
}

extern "C" void runtime_machine_first_fault_observation_fixture_stop(int enabled)
{
  runtime_machine_first_fault_observation_enable(enabled);
}

extern "C" int runtime_machine_first_fault_observation_observed(void)
{
  return runtime_first_fault_was_observed;
}

extern "C" int runtime_machine_first_fault_observation_copy(
  struct runtime_first_fault_observation *event)
{
  if (event == 0 || !runtime_first_fault_was_observed) return 0;
  *event = runtime_first_fault_event;
  return 1;
}

extern "C" int runtime_machine_first_fault_observation(
  const struct runtime_first_fault_observation *event)
{
  if (!runtime_first_fault_fixture_enabled || runtime_first_fault_was_observed ||
      event == 0 || event->magic != RUNTIME_FIRST_FAULT_OBSERVATION_MAGIC ||
      event->abi_version != RUNTIME_FIRST_FAULT_OBSERVATION_VERSION ||
      event->struct_bytes != sizeof(*event) || event->vector == 6u) return 0;
  runtime_first_fault_event = *event;
  runtime_first_fault_was_observed = 1;
  return 1;
}