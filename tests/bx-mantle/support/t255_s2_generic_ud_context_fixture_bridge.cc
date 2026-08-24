#include "t255_s2_generic_ud_context_fixture_bridge.h"

#include <string.h>

static struct t255_s2_generic_ud_context_fixture t255_context;
static int t255_stop_observed;
static int t255_pending_observed;

void t255_s2_generic_ud_context_fixture_clear(
  struct t255_s2_generic_ud_context_fixture *fixture)
{
  if (fixture == 0) return;
  memset(fixture, 0, sizeof(*fixture));
  fixture->magic = T255_S2_GENERIC_UD_CONTEXT_FIXTURE_MAGIC;
  fixture->abi_version = RUNTIME_GENERIC_UD_OUTCOME_V2_VERSION;
  fixture->struct_bytes = sizeof(*fixture);
}

int t255_s2_generic_ud_context_fixture_configure(
  const struct t255_s2_generic_ud_context_fixture *fixture)
{
  if (fixture == 0 ||
      fixture->magic != T255_S2_GENERIC_UD_CONTEXT_FIXTURE_MAGIC ||
      fixture->abi_version != RUNTIME_GENERIC_UD_OUTCOME_V2_VERSION ||
      fixture->struct_bytes != sizeof(*fixture) ||
      fixture->expected_execution_mode > 3u ||
      fixture->outcome.abi_version != RUNTIME_GENERIC_UD_OUTCOME_V2_VERSION ||
      fixture->outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
      fixture->outcome.gpr32_write_mask > 0xffu ||
      fixture->outcome.segment_write_mask > 0x3fu ||
      (fixture->outcome.eflags_write_mask &
        ~RUNTIME_GENERIC_UD_EFLAGS_WRITE_MASK) != 0u ||
      fixture->outcome.context_mode > RUNTIME_GENERIC_UD_CONTEXT_PROTECTED)
    return 0;
  t255_context = *fixture;
  return 1;
}

extern "C" int runtime_mantle_generic_ud_bridge_v2(
  const struct runtime_generic_ud_event_v1 *event,
  struct runtime_generic_ud_outcome_v2 *outcome)
{
  if (event != 0 && outcome != 0 &&
      event->magic == RUNTIME_GENERIC_UD_EVENT_V1_MAGIC &&
      event->abi_version == RUNTIME_GENERIC_UD_EVENT_V1_VERSION &&
      event->struct_bytes == sizeof(*event) && event->vector == 6u &&
      t255_context.magic == T255_S2_GENERIC_UD_CONTEXT_FIXTURE_MAGIC &&
      t255_context.expected_execution_mode == event->execution_mode) {
    *outcome = t255_context.outcome;
    t255_context.magic = 0u;
    return 1;
  }
  return 0;
}

extern "C" void runtime_mantle_generic_ud_stop_observation_reset(void)
{
  t255_stop_observed = 0;
}

extern "C" void runtime_mantle_generic_ud_stop_observation_mark(void)
{
  t255_stop_observed = 1;
}

extern "C" int runtime_mantle_generic_ud_stop_observed(void)
{
  return t255_stop_observed;
}

extern "C" void runtime_mantle_generic_ud_pending_observation_reset(void)
{
  t255_pending_observed = 0;
}

extern "C" void runtime_mantle_generic_ud_pending_observation_mark(void)
{
  t255_pending_observed = 1;
}

extern "C" int runtime_mantle_generic_ud_pending_observed(void)
{
  return t255_pending_observed;
}
