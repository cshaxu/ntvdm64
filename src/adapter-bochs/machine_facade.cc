#include "bochs.h"
#include "pc_system.h"
#include "machine_facade.h"

extern "C" int machine_facade_v1_get_a20(uint32_t *enabled)
{
  if (enabled == 0) return 0;
  *enabled = bx_pc_system.get_enable_a20() ? 1u : 0u;
  return 1;
}

extern "C" int machine_facade_v1_set_a20(uint32_t enabled)
{
  if (enabled > 1u) return 0;
  bx_pc_system.set_enable_a20(enabled ? 1 : 0);
  return 1;
}
