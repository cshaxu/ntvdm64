/////////////////////////////////////////////////////////////////////////
//
// Default-off first non-UD exception observer.  It holds one copied record
// only for a focused finite-machine diagnostic.
//
/////////////////////////////////////////////////////////////////////////

#include "bx_ntvdm_first_fault_observation_v1.h"

#include <string.h>

static int bx_ntvdm_first_fault_fixture_enabled;
static int bx_ntvdm_first_fault_was_observed;
static struct bx_ntvdm_first_fault_observation_v1 bx_ntvdm_first_fault_event;

extern "C" void bx_ntvdm_mantle_first_fault_observation_reset(void)
{
  bx_ntvdm_first_fault_was_observed = 0;
  memset(&bx_ntvdm_first_fault_event, 0, sizeof(bx_ntvdm_first_fault_event));
}

extern "C" void bx_ntvdm_mantle_first_fault_observation_fixture_stop(int enabled)
{
  if (enabled) bx_ntvdm_mantle_first_fault_observation_reset();
  bx_ntvdm_first_fault_fixture_enabled = enabled != 0;
}

extern "C" int bx_ntvdm_mantle_first_fault_observation_observed(void)
{
  return bx_ntvdm_first_fault_was_observed;
}

extern "C" int bx_ntvdm_mantle_first_fault_observation_copy(
  struct bx_ntvdm_first_fault_observation_v1 *event)
{
  if (event == 0 || !bx_ntvdm_first_fault_was_observed) return 0;
  *event = bx_ntvdm_first_fault_event;
  return 1;
}

extern "C" int bx_ntvdm_mantle_first_fault_observation_v1(
  const struct bx_ntvdm_first_fault_observation_v1 *event)
{
  if (!bx_ntvdm_first_fault_fixture_enabled || bx_ntvdm_first_fault_was_observed ||
      event == 0 || event->magic != BX_NTVDM_FIRST_FAULT_OBSERVATION_V1_MAGIC ||
      event->abi_version != BX_NTVDM_FIRST_FAULT_OBSERVATION_V1_VERSION ||
      event->struct_bytes != sizeof(*event) || event->vector == 6u) return 0;
  bx_ntvdm_first_fault_event = *event;
  bx_ntvdm_first_fault_was_observed = 1;
  return 1;
}