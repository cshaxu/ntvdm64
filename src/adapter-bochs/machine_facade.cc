#include "bochs.h"
#include "bochs-core/memory/memory.h"
#include "bochs-core/cpu/cpu.h"
#include "bochs-core/cpu/opaque_callback.h"
#include "bochs-core/pc_system.h"
#include "machine_facade.h"
#include "minimal_machine.h"

static bx_mantle_minimal_machine_c *machine_facade_machine;

extern "C" int machine_facade_v1_bind_opaque_callback(
  machine_facade_v1_opaque_callback callback, void *context)
{
  return bochs_core_opaque_callback_v1_bind(
    (bochs_core_opaque_callback_v1) callback, context);
}

extern "C" void machine_facade_v1_unbind_opaque_callback(void)
{
  bochs_core_opaque_callback_v1_unbind();
}

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
  uint64_t byte_count, const uint8_t *bytes)
{
  return bytes != 0 && bx_mem.copy_to_ordinary_ram((bx_phy_address)address,
    (Bit64u)byte_count, (uint8_t *)bytes) ? 1 : 0;
}

extern "C" int machine_facade_v1_execute_protected_range(uint32_t kind,
  uint32_t segment, uint32_t offset, uint32_t byte_count, uint8_t *bytes)
{
  bx_address linear;
  bx_bool access_ok;
  if (bytes == 0 || segment >= 6u || byte_count == 0u ||
      (kind != 1u && kind != 2u) || !bx_cpu.protected_mode() ||
      (bx_cpu.read_CR0() & 0x80000000u) != 0u)
    return MACHINE_FACADE_V1_PROTECTED_RANGE_REJECTED_MODE;
  access_ok = kind == 1u ? bx_cpu.read_virtual_checks(
    &bx_cpu.sregs[segment], offset, byte_count) : bx_cpu.write_virtual_checks(
      &bx_cpu.sregs[segment], offset, byte_count);
  if (!access_ok) return MACHINE_FACADE_V1_PROTECTED_RANGE_REJECTED_ACCESS;
  linear = bx_cpu.get_laddr32(segment, offset);
  if (!bx_pc_system.get_enable_a20() &&
      ((((Bit64u)linear & 0x100000u) != 0u) ||
       (Bit64u)linear + byte_count > 0x100000u))
    return MACHINE_FACADE_V1_PROTECTED_RANGE_REJECTED_MEMORY;
  if (!(kind == 1u ? machine_facade_v1_memory_read(linear, byte_count, bytes) :
      machine_facade_v1_memory_write(linear, byte_count, bytes)))
    return MACHINE_FACADE_V1_PROTECTED_RANGE_REJECTED_MEMORY;
  return MACHINE_FACADE_V1_PROTECTED_RANGE_OK;
}

extern "C" int machine_facade_v1_machine_begin(uint64_t guest_bytes,
  uint64_t host_bytes)
{
  if (machine_facade_machine != 0) return 0;
  machine_facade_machine = new bx_mantle_minimal_machine_c;
  if (machine_facade_machine == 0 ||
      machine_facade_machine->initialize((Bit64u)guest_bytes,
        (Bit64u)host_bytes) != BX_MANTLE_MINIMAL_MACHINE_OK) {
    delete machine_facade_machine;
    machine_facade_machine = 0;
    return 0;
  }
  return 1;
}

extern "C" int machine_facade_v1_machine_cleanup(void)
{
  bx_mantle_minimal_machine_c *machine = machine_facade_machine;
  machine_facade_machine = 0;
  if (machine == 0) return 1;
  if (machine->cleanup() != BX_MANTLE_MINIMAL_MACHINE_OK) {
    delete machine;
    return 0;
  }
  delete machine;
  return 1;
}

extern "C" int machine_facade_v1_machine_active(void)
{
  return machine_facade_machine != 0;
}

extern "C" int machine_facade_v1_set_realmode_segment_limit_compatibility(
  uint32_t enabled)
{
  return machine_facade_machine != 0 && enabled <= 1u &&
    machine_facade_machine->set_realmode_segment_limit_compatibility(
      enabled ? 1 : 0) ? 1 : 0;
}

extern "C" int machine_facade_v1_compose_headless_8042(void)
{
  return machine_facade_machine != 0 &&
    machine_facade_machine->compose_headless_8042() ==
      BX_MANTLE_MINIMAL_MACHINE_OK ? 1 : 0;
}

extern "C" void machine_facade_v1_initialize_timing(uint32_t ips)
{
  bx_pc_system.initialize((unsigned)ips);
}

extern "C" void machine_facade_v1_apply_real_mode_entry(uint16_t cs,
  uint32_t eip)
{
  bx_cpu.apply_real_mode_entry(cs, eip);
}

extern "C" int machine_facade_v1_copy_real_mode_entry(uint16_t *cs,
  uint32_t *eip)
{
  if (cs == 0 || eip == 0) return 0;
  *cs = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
  *eip = bx_cpu.get_eip();
  return 1;
}

extern "C" int machine_facade_v1_copy_real_mode_state(uint16_t *cs,
  uint16_t *ss, uint16_t *sp, uint32_t *eip)
{
  if (cs == 0 || ss == 0 || sp == 0 || eip == 0) return 0;
  *cs = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
  *ss = bx_cpu.sregs[BX_SEG_REG_SS].selector.value;
  *sp = bx_cpu.get_reg16(BX_16BIT_REG_SP);
  *eip = bx_cpu.get_eip();
  return 1;
}

extern "C" void machine_facade_v1_request_cpu_stop(void)
{
  bx_pc_system.kill_bochs_request = 1;
  bx_cpu.async_event = 1;
}

extern "C" void machine_facade_v1_cpu_loop(void)
{
  bx_cpu.cpu_loop();
}

extern "C" int machine_facade_v1_register_timer(void *opaque,
  machine_facade_v1_timer_callback callback, uint64_t ticks,
  uint32_t continuous, uint32_t active, uint32_t *timer_id)
{
  int id;
  if (callback == 0 || timer_id == 0 || continuous > 1u || active > 1u)
    return 0;
  id = bx_pc_system.register_timer_ticks(opaque, callback, (Bit64u)ticks,
    continuous ? 1 : 0, active ? 1 : 0, "adapter-bochs-timer");
  if (id <= 0) return 0;
  *timer_id = (uint32_t)id;
  return 1;
}

extern "C" void machine_facade_v1_deactivate_timer(uint32_t timer_id)
{
  bx_pc_system.deactivate_timer((unsigned)timer_id);
}

extern "C" void machine_facade_v1_unregister_timer(uint32_t timer_id)
{
  bx_pc_system.unregisterTimer((unsigned)timer_id);
}
