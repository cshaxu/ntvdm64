/////////////////////////////////////////////////////////////////////////
//
// Default finite-mantle #UD bridge.  A build that does not explicitly compose
// a consumer declines and therefore preserves the native exception path.
//
/////////////////////////////////////////////////////////////////////////

#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"

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

/* A normal composed build deliberately has no v2 consumer yet.  This remains
 * a pure default decline; context-injection state belongs only to its
 * machine-test supplemental object. */
extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v2(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v2 *outcome)
{
  (void)event;
  (void)outcome;
  return 0;
}
