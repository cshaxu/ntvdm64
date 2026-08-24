#include "bochs.h"
#include "bx-core/cpu/cpu.h"
#include "bx-core/memory/memory.h"
#include "bx-mantle/minimal_machine.h"
#include "adapter-softpc/bx_ntvdm_physical_write_observation_v1.h"

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *,
  struct bx_ntvdm_generic_ud_outcome_v1 *)
{ return 0; }

int main()
{
  bx_mantle_minimal_machine_c machine;
  struct bx_ntvdm_physical_write_observation_v1 value;
  Bit8u first = 0x5a, second[2] = {0xa5, 0x3c}, actual[2] = {0, 0};

  bx_ntvdm_physical_write_observation_v1_reset();
  if (bx_ntvdm_physical_write_observation_v1_configure(0x200u, 0u) ||
      bx_ntvdm_physical_write_observation_v1_copy(&value)) return 1;
  if (machine.initialize(0x200000u, 0x200000u) != BX_MANTLE_MINIMAL_MACHINE_OK)
    return 2;
  bx_cpu.sregs[BX_SEG_REG_CS].selector.value = 0x1234u;
  bx_cpu.sregs[BX_SEG_REG_CS].cache.u.segment.base = 0x12340u;
  bx_cpu.sregs[BX_SEG_REG_SS].selector.value = 0x5678u;
  bx_cpu.prev_rip = 0x9abcu;
  bx_cpu.icount = 0x11223344u;
  bx_cpu.set_reg16(BX_16BIT_REG_SP, 0xdef0u);
  bx_mem.writePhysicalPage(&bx_cpu, 0x200u, 1u, &first);
  if (bx_ntvdm_physical_write_observation_v1_copy(&value) ||
      !bx_mem.copy_from_ordinary_ram(0x200u, 1u, actual) || actual[0] != first)
    return 3;
  if (!bx_ntvdm_physical_write_observation_v1_configure(0x300u, 2u)) return 4;
  bx_mem.writePhysicalPage(&bx_cpu, 0x200u, 1u, &first);
  if (bx_ntvdm_physical_write_observation_v1_copy(&value)) return 5;
  bx_mem.writePhysicalPage(&bx_cpu, 0x301u, 2u, second);
  if (!bx_ntvdm_physical_write_observation_v1_copy(&value) ||
      value.magic != BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_MAGIC ||
      value.abi_version != BX_NTVDM_PHYSICAL_WRITE_OBSERVATION_V1_VERSION ||
      value.physical_address != 0x301u || value.byte_count != 2u ||
      value.captured_bytes != 2u || value.bytes[0] != second[0] ||
      value.bytes[1] != second[1] || value.sequence != 0x11223344u ||
      value.rip != 0x9abcu || value.cs_base != 0x12340u ||
      value.cs != 0x1234u || value.ss != 0x5678u ||
      value.sp != 0xdef0u || !bx_mem.copy_from_ordinary_ram(0x301u, 2u, actual) ||
      actual[0] != second[0] || actual[1] != second[1]) return 6;
  first = 0x77;
  bx_mem.writePhysicalPage(&bx_cpu, 0x300u, 1u, &first);
  if (!bx_ntvdm_physical_write_observation_v1_copy(&value) ||
      value.physical_address != 0x301u || value.bytes[0] != second[0]) return 7;
  bx_ntvdm_physical_write_observation_v1_reset();
  if (bx_ntvdm_physical_write_observation_v1_copy(&value)) return 8;
  return machine.cleanup() == BX_MANTLE_MINIMAL_MACHINE_OK ? 0 : 9;
}