#include "bochs.h"
#include "bochs-core/memory/memory.h"
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

extern "C" int machine_facade_v1_memory_readable(uint64_t address,
  uint64_t byte_count)
{
  return bx_mem.ordinary_ram_readable((bx_phy_address)address,
    (Bit64u)byte_count) ? 1 : 0;
}

extern "C" int machine_facade_v1_memory_writable(uint64_t address,
  uint64_t byte_count)
{
  return bx_mem.ordinary_ram_writable((bx_phy_address)address,
    (Bit64u)byte_count) ? 1 : 0;
}

extern "C" int machine_facade_v1_memory_read(uint64_t address,
  uint64_t byte_count, uint8_t *bytes)
{
  return bytes != 0 && bx_mem.copy_from_ordinary_ram((bx_phy_address)address,
    (Bit64u)byte_count, bytes) ? 1 : 0;
}

extern "C" int machine_facade_v1_memory_write(uint64_t address,
  uint64_t byte_count, uint8_t *bytes)
{
  return bytes != 0 && bx_mem.copy_to_ordinary_ram((bx_phy_address)address,
    (Bit64u)byte_count, bytes) ? 1 : 0;
}
