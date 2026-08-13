/////////////////////////////////////////////////////////////////////////
//
// Default finite-mantle #UD bridge.  A build that does not explicitly compose
// a consumer declines and therefore preserves the native exception path.
//
/////////////////////////////////////////////////////////////////////////

#include "bx_ntvdm_generic_ud_bridge.h"

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
  (void) event;
  (void) outcome;
  return 0;
}
