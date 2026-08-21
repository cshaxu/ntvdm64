/////////////////////////////////////////////////////////////////////////
//
// Default finite-mantle #UD bridge.  A build that does not explicitly compose
// a consumer declines and therefore preserves the native exception path.
//
/////////////////////////////////////////////////////////////////////////

#include "bx_ntvdm_generic_ud_bridge.h"

static int bx_ntvdm_generic_ud_fixture_stop_enabled = 0;
static int bx_ntvdm_generic_ud_fixture_stop_was_observed = 0;
static int bx_ntvdm_generic_ud_stop_was_observed = 0;
static int bx_ntvdm_generic_ud_pending_was_observed = 0;

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
