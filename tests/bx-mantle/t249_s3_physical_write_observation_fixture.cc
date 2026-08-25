#include "bochs.h"
#include "bochs-core/cpu/cpu.h"
#include "bochs-core/memory/memory.h"
#include "adapter-bochs/minimal_machine.h"
#include "adapter-softpc/physical_write_observation.h"
#include "adapter-bochs/machine_facade.h"

static int observe_physical_write(void *, const void *event, unsigned event_bytes,
  void *, unsigned)
{
  const struct runtime_physical_write_observation_v1 *record;
  uint32_t tag;
  if (event == 0 || event_bytes != sizeof(*record) + sizeof(tag)) return 0;
  record = (const struct runtime_physical_write_observation_v1 *)event;
  memcpy(&tag, (const uint8_t *)event + sizeof(*record), sizeof(tag));
  if (record->magic != RUNTIME_PHYSICAL_WRITE_OBSERVATION_V1_MAGIC ||
      tag != 0x42585057u) return 0;
  runtime_physical_write_observation_v1_record(record->physical_address,
    record->byte_count, record->bytes, record->sequence, record->rip,
    record->cs_base, record->cs, record->ss, record->sp);
  return 1;
}
int main()
{
  bx_mantle_minimal_machine_c machine;
  struct runtime_physical_write_observation_v1 value;
  Bit8u first = 0x5a, second[2] = {0xa5, 0x3c}, actual[2] = {0, 0};

  runtime_physical_write_observation_v1_reset();
  if (!machine_facade_v1_bind_opaque_callback(observe_physical_write, 0)) return 10;
  if (runtime_physical_write_observation_v1_configure(0x200u, 0u) ||
      runtime_physical_write_observation_v1_copy(&value)) return 1;
  if (machine.initialize(0x200000u, 0x200000u) != BX_MANTLE_MINIMAL_MACHINE_OK)
    return 2;
  bx_cpu.sregs[BX_SEG_REG_CS].selector.value = 0x1234u;
  bx_cpu.sregs[BX_SEG_REG_CS].cache.u.segment.base = 0x12340u;
  bx_cpu.sregs[BX_SEG_REG_SS].selector.value = 0x5678u;
  bx_cpu.prev_rip = 0x9abcu;
  bx_cpu.icount = 0x11223344u;
  bx_cpu.set_reg16(BX_16BIT_REG_SP, 0xdef0u);
  bx_mem.writePhysicalPage(&bx_cpu, 0x200u, 1u, &first);
  if (runtime_physical_write_observation_v1_copy(&value) ||
      !bx_mem.copy_from_ordinary_ram(0x200u, 1u, actual) || actual[0] != first)
    return 3;
  if (!runtime_physical_write_observation_v1_configure(0x300u, 2u)) return 4;
  bx_mem.writePhysicalPage(&bx_cpu, 0x200u, 1u, &first);
  if (runtime_physical_write_observation_v1_copy(&value)) return 5;
  bx_mem.writePhysicalPage(&bx_cpu, 0x301u, 2u, second);
  if (!runtime_physical_write_observation_v1_copy(&value) ||
      value.magic != RUNTIME_PHYSICAL_WRITE_OBSERVATION_V1_MAGIC ||
      value.abi_version != RUNTIME_PHYSICAL_WRITE_OBSERVATION_V1_VERSION ||
      value.physical_address != 0x301u || value.byte_count != 2u ||
      value.captured_bytes != 2u || value.bytes[0] != second[0] ||
      value.bytes[1] != second[1] || value.sequence != 0x11223344u ||
      value.rip != 0x9abcu || value.cs_base != 0x12340u ||
      value.cs != 0x1234u || value.ss != 0x5678u ||
      value.sp != 0xdef0u || !bx_mem.copy_from_ordinary_ram(0x301u, 2u, actual) ||
      actual[0] != second[0] || actual[1] != second[1]) return 6;
  first = 0x77;
  bx_mem.writePhysicalPage(&bx_cpu, 0x300u, 1u, &first);
  if (!runtime_physical_write_observation_v1_copy(&value) ||
      value.physical_address != 0x301u || value.bytes[0] != second[0]) return 7;
  runtime_physical_write_observation_v1_reset();
  if (runtime_physical_write_observation_v1_copy(&value)) return 8;
  machine_facade_v1_unbind_opaque_callback();
  return machine.cleanup() == BX_MANTLE_MINIMAL_MACHINE_OK ? 0 : 9;
}