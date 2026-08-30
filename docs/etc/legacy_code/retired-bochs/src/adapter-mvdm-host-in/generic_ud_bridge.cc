/////////////////////////////////////////////////////////////////////////
//
// Default finite-machine #UD bridge.  A build that does not explicitly compose
// a consumer declines and therefore preserves the native exception path.
//
/////////////////////////////////////////////////////////////////////////

#include "generic_ud_bridge.h"

static int runtime_generic_ud_stop_was_observed = 0;
static int runtime_generic_ud_pending_was_observed = 0;

extern "C" void runtime_machine_generic_ud_stop_observation_reset(void)
{
  runtime_generic_ud_stop_was_observed = 0;
}

extern "C" void runtime_machine_generic_ud_stop_observation_mark(void)
{
  runtime_generic_ud_stop_was_observed = 1;
}

extern "C" int runtime_machine_generic_ud_stop_observed(void)
{
  return runtime_generic_ud_stop_was_observed;
}

extern "C" void runtime_machine_generic_ud_pending_observation_reset(void)
{
  runtime_generic_ud_pending_was_observed = 0;
}

extern "C" void runtime_machine_generic_ud_pending_observation_mark(void)
{
  runtime_generic_ud_pending_was_observed = 1;
}

extern "C" int runtime_machine_generic_ud_pending_observed(void)
{
  return runtime_generic_ud_pending_was_observed;
}

/* A normal composed build deliberately has no context consumer yet.  This remains
 * a pure default decline; context-injection state belongs only to its
 * machine-test supplemental object. */
extern "C" int runtime_machine_generic_ud_context_bridge(
  const struct runtime_generic_ud_event *event,
  struct runtime_generic_ud_context_outcome *outcome)
{
  (void)event;
  (void)outcome;
  return 0;
}
