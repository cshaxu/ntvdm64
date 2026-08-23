#include "bx_ntvdm_segment_access_observation_v1.h"
#include "bx_ntvdm_first_fault_observation_v1.h"

#include <string.h>

static int enabled;
static int observed;
static struct bx_ntvdm_segment_access_observation_v1 saved;

extern "C" void bx_ntvdm_mantle_segment_access_observation_reset(void)
{
  observed = 0;
  memset(&saved, 0, sizeof(saved));
}

extern "C" void bx_ntvdm_mantle_segment_access_observation_enable(int value)
{
  if (value) bx_ntvdm_mantle_segment_access_observation_reset();
  enabled = value != 0;
}

extern "C" int bx_ntvdm_mantle_segment_access_observation_copy(
  struct bx_ntvdm_segment_access_observation_v1 *event)
{
  if (event == 0 || !observed) return 0;
  *event = saved;
  return 1;
}

extern "C" int bx_ntvdm_mantle_segment_access_observation_v1(
  const struct bx_ntvdm_segment_access_observation_v1 *event)
{
  if (!enabled || observed ||
      !bx_ntvdm_mantle_first_fault_observation_enabled() || event == 0 ||
      event->magic != BX_NTVDM_SEGMENT_ACCESS_OBSERVATION_V1_MAGIC ||
      event->abi_version != BX_NTVDM_SEGMENT_ACCESS_OBSERVATION_V1_VERSION ||
      event->struct_bytes != sizeof(*event) ||
      event->access_kind != BX_NTVDM_SEGMENT_ACCESS_KIND_V1_READ_WORD ||
      (event->branch_kind != BX_NTVDM_SEGMENT_ACCESS_BRANCH_V1_DIRECT_LIMIT &&
       event->branch_kind != BX_NTVDM_SEGMENT_ACCESS_BRANCH_V1_READ_CHECK) ||
      event->width != 2u) return 0;
  saved = *event;
  observed = 1;
  return 1;
}
