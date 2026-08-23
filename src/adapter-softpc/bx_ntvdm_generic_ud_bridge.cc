/////////////////////////////////////////////////////////////////////////
//
// Default finite-mantle #UD bridge.  A build that does not explicitly compose
// a consumer declines and therefore preserves the native exception path.
//
/////////////////////////////////////////////////////////////////////////

#include "bx_ntvdm_generic_ud_bridge.h"

#include <string.h>

static int bx_ntvdm_generic_ud_fixture_stop_enabled = 0;
static int bx_ntvdm_generic_ud_fixture_stop_was_observed = 0;
static int bx_ntvdm_generic_ud_stop_was_observed = 0;
static int bx_ntvdm_generic_ud_pending_was_observed = 0;
static struct bx_ntvdm_generic_ud_context_fixture_v2
  bx_ntvdm_generic_ud_context_fixture_v2_state;

extern "C" void bx_ntvdm_mantle_generic_ud_context_fixture_v2_clear(
  struct bx_ntvdm_generic_ud_context_fixture_v2 *fixture)
{
  if (fixture == 0) return;
  memset(fixture, 0, sizeof(*fixture));
  fixture->magic = BX_NTVDM_GENERIC_UD_CONTEXT_FIXTURE_V2_MAGIC;
  fixture->abi_version = BX_NTVDM_GENERIC_UD_OUTCOME_V2_VERSION;
  fixture->struct_bytes = sizeof(*fixture);
}

extern "C" int bx_ntvdm_mantle_generic_ud_context_fixture_v2_configure(
  const struct bx_ntvdm_generic_ud_context_fixture_v2 *fixture)
{
  if (fixture == 0 ||
      fixture->magic != BX_NTVDM_GENERIC_UD_CONTEXT_FIXTURE_V2_MAGIC ||
      fixture->abi_version != BX_NTVDM_GENERIC_UD_OUTCOME_V2_VERSION ||
      fixture->struct_bytes != sizeof(*fixture) ||
      fixture->expected_execution_mode > 3u ||
      fixture->outcome.abi_version != BX_NTVDM_GENERIC_UD_OUTCOME_V2_VERSION ||
      fixture->outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
      fixture->outcome.gpr32_write_mask > 0xffu ||
      fixture->outcome.segment_write_mask > 0x3fu ||
      (fixture->outcome.eflags_write_mask &
        ~BX_NTVDM_GENERIC_UD_EFLAGS_WRITE_MASK) != 0u ||
      fixture->outcome.context_mode > BX_NTVDM_GENERIC_UD_CONTEXT_PROTECTED)
    return 0;
  bx_ntvdm_generic_ud_context_fixture_v2_state = *fixture;
  return 1;
}

extern "C" void bx_ntvdm_mantle_generic_ud_stop_observation_reset(void)
{
  bx_ntvdm_generic_ud_stop_was_observed = 0;
}

extern "C" void bx_ntvdm_mantle_generic_ud_stop_observation_mark(void)
{
  bx_ntvdm_generic_ud_stop_was_observed = 1;
}

extern "C" int bx_ntvdm_mantle_generic_ud_stop_observed(void)
{
  return bx_ntvdm_generic_ud_stop_was_observed;
}

extern "C" void bx_ntvdm_mantle_generic_ud_pending_observation_reset(void)
{
  bx_ntvdm_generic_ud_pending_was_observed = 0;
}

extern "C" void bx_ntvdm_mantle_generic_ud_pending_observation_mark(void)
{
  bx_ntvdm_generic_ud_pending_was_observed = 1;
}

extern "C" int bx_ntvdm_mantle_generic_ud_pending_observed(void)
{
  return bx_ntvdm_generic_ud_pending_was_observed;
}

extern "C" void bx_ntvdm_mantle_generic_ud_fixture_stop(int enabled)
{
  if (enabled) bx_ntvdm_generic_ud_fixture_stop_was_observed = 0;
  bx_ntvdm_generic_ud_fixture_stop_enabled = enabled != 0;
}

extern "C" int bx_ntvdm_mantle_generic_ud_fixture_stop_observed(void)
{
  return bx_ntvdm_generic_ud_fixture_stop_was_observed;
}

#ifndef BX_NTVDM_EXTERNAL_GENERIC_UD_BRIDGE
extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
  if (event == 0 || outcome == 0 ||
      event->magic != BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC ||
      event->abi_version != BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION ||
      event->struct_bytes != sizeof(*event) || event->vector != 6u ||
      !bx_ntvdm_generic_ud_fixture_stop_enabled) return 0;
  outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
  outcome->disposition = BX_NTVDM_GENERIC_UD_STOP;
  bx_ntvdm_mantle_generic_ud_stop_observation_mark();
  bx_ntvdm_generic_ud_fixture_stop_was_observed = 1;
  return 1;
}
#endif

/* A normal composed build deliberately has no v2 consumer yet.  Retain the
 * source-compatible decline until a later selector-blind consumer is
 * explicitly composed. */
extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v2(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v2 *outcome)
{
  if (event != 0 && outcome != 0 &&
      event->magic == BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC &&
      event->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
      event->struct_bytes == sizeof(*event) && event->vector == 6u &&
      bx_ntvdm_generic_ud_context_fixture_v2_state.magic ==
        BX_NTVDM_GENERIC_UD_CONTEXT_FIXTURE_V2_MAGIC &&
      bx_ntvdm_generic_ud_context_fixture_v2_state.expected_execution_mode ==
        event->execution_mode) {
    *outcome = bx_ntvdm_generic_ud_context_fixture_v2_state.outcome;
    bx_ntvdm_generic_ud_context_fixture_v2_state.magic = 0u;
    return 1;
  }
  return 0;
}
