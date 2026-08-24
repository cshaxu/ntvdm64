#include "segment_access_observation.h"
#include "first_fault_observation.h"

#include <string.h>

static int enabled;
static int observed;
static struct runtime_segment_access_observation_v1 saved;

extern "C" void runtime_mantle_segment_access_observation_reset(void)
{
  observed = 0;
  memset(&saved, 0, sizeof(saved));
}

extern "C" void runtime_mantle_segment_access_observation_enable(int value)
{
  if (value) runtime_mantle_segment_access_observation_reset();
  enabled = value != 0;
}

extern "C" int runtime_mantle_segment_access_observation_copy(
  struct runtime_segment_access_observation_v1 *event)
{
  if (event == 0 || !observed) return 0;
  *event = saved;
  return 1;
}

extern "C" int runtime_mantle_segment_access_observation_v1(
  const struct runtime_segment_access_observation_v1 *event)
{
  if (!enabled || observed ||
      !runtime_mantle_first_fault_observation_enabled() || event == 0 ||
      event->magic != RUNTIME_SEGMENT_ACCESS_OBSERVATION_V1_MAGIC ||
      event->abi_version != RUNTIME_SEGMENT_ACCESS_OBSERVATION_V1_VERSION ||
      event->struct_bytes != sizeof(*event) ||
      event->access_kind != RUNTIME_SEGMENT_ACCESS_KIND_V1_READ_WORD ||
      (event->branch_kind != RUNTIME_SEGMENT_ACCESS_BRANCH_V1_DIRECT_LIMIT &&
       event->branch_kind != RUNTIME_SEGMENT_ACCESS_BRANCH_V1_READ_CHECK) ||
      event->width != 2u) return 0;
  saved = *event;
  observed = 1;
  return 1;
}
