#include "bochs.h"
#include "bochs-core/memory/memory.h"
#include "bochs-core/cpu/cpu.h"
#include "bochs-core/pc_system.h"
#include "machine_facade.h"
#include "minimal_machine.h"

#include <string.h>

static adapter_bochs_minimal_machine_c *machine_facade_machine;
/* A copied-frame commit is legal only while adapter-bochs owns a returned
 * CPU loop.  This is lifecycle state, not a guest/VDM scheduler. */
static int machine_facade_cpu_paused;

extern "C" int machine_facade_bind_opaque_callback(
  machine_facade_opaque_callback callback, void *context)
{
  return bx_cpu.overlay_bind_opaque_callback(
    (bx_cpu_opaque_callback_t) callback, context);
}

extern "C" void machine_facade_unbind_opaque_callback(void)
{
  bx_cpu.overlay_unbind_opaque_callback();
}

extern "C" int machine_facade_get_a20(uint32_t *enabled)
{
  if (enabled == 0) return 0;
  *enabled = bx_pc_system.get_enable_a20() ? 1u : 0u;
  return 1;
}

extern "C" int machine_facade_set_a20(uint32_t enabled)
{
  if (enabled > 1u) return 0;
  bx_pc_system.set_enable_a20(enabled ? 1 : 0);
  return 1;
}

extern "C" int machine_facade_memory_readable(uint64_t address,
  uint64_t byte_count)
{
  return bx_mem.ordinary_ram_readable((bx_phy_address)address,
    (Bit64u)byte_count) ? 1 : 0;
}

extern "C" int machine_facade_memory_writable(uint64_t address,
  uint64_t byte_count)
{
  return bx_mem.ordinary_ram_writable((bx_phy_address)address,
    (Bit64u)byte_count) ? 1 : 0;
}

extern "C" int machine_facade_memory_read(uint64_t address,
  uint64_t byte_count, uint8_t *bytes)
{
  return bytes != 0 && bx_mem.copy_from_ordinary_ram((bx_phy_address)address,
    (Bit64u)byte_count, bytes) ? 1 : 0;
}

extern "C" int machine_facade_memory_write(uint64_t address,
  uint64_t byte_count, const uint8_t *bytes)
{
  return bytes != 0 && bx_mem.copy_to_ordinary_ram((bx_phy_address)address,
    (Bit64u)byte_count, (uint8_t *)bytes) ? 1 : 0;
}

extern "C" int machine_facade_resolve_protected_range(uint32_t kind,
  uint32_t segment, uint32_t offset, uint32_t byte_count, uint32_t *linear_out)
{
  bx_address linear;
  bx_bool access_ok;
  if (linear_out == 0 || segment >= 6u || byte_count == 0u ||
      (kind != 1u && kind != 2u) || !bx_cpu.protected_mode() ||
      (bx_cpu.read_CR0() & 0x80000000u) != 0u)
    return MACHINE_FACADE_PROTECTED_RANGE_REJECTED_MODE;
  access_ok = kind == 1u ? bx_cpu.read_virtual_checks(
    &bx_cpu.sregs[segment], offset, byte_count) : bx_cpu.write_virtual_checks(
      &bx_cpu.sregs[segment], offset, byte_count);
  if (!access_ok) return MACHINE_FACADE_PROTECTED_RANGE_REJECTED_ACCESS;
  linear = bx_cpu.get_laddr32(segment, offset);
  if (!bx_pc_system.get_enable_a20() &&
      ((((Bit64u)linear & 0x100000u) != 0u) ||
       (Bit64u)linear + byte_count > 0x100000u))
    return MACHINE_FACADE_PROTECTED_RANGE_REJECTED_MEMORY;
  if ((Bit64u)linear > UINT32_MAX) return MACHINE_FACADE_PROTECTED_RANGE_REJECTED_MEMORY;
  *linear_out = (uint32_t)linear;
  return MACHINE_FACADE_PROTECTED_RANGE_OK;
}

extern "C" int machine_facade_execute_protected_range(uint32_t kind,
  uint32_t segment, uint32_t offset, uint32_t byte_count, uint8_t *bytes)
{
  uint32_t linear;
  if (bytes == 0 || machine_facade_resolve_protected_range(kind, segment,
      offset, byte_count, &linear) != MACHINE_FACADE_PROTECTED_RANGE_OK)
    return MACHINE_FACADE_PROTECTED_RANGE_REJECTED_MEMORY;
  if (!(kind == 1u ? machine_facade_memory_read(linear, byte_count, bytes) :
      machine_facade_memory_write(linear, byte_count, bytes)))
    return MACHINE_FACADE_PROTECTED_RANGE_REJECTED_MEMORY;
  return MACHINE_FACADE_PROTECTED_RANGE_OK;
}

extern "C" void machine_facade_protected_frame_clear(
  struct machine_facade_protected_frame *frame)
{
  if (frame == 0) return;
  memset(frame, 0, sizeof(*frame));
  frame->abi_version = MACHINE_FACADE_PROTECTED_FRAME_VERSION;
  frame->struct_bytes = sizeof(*frame);
}

extern "C" int machine_facade_protected_frame_valid(
  const struct machine_facade_protected_frame *frame)
{
  return frame != 0 &&
    frame->abi_version == MACHINE_FACADE_PROTECTED_FRAME_VERSION &&
    frame->struct_bytes == sizeof(*frame) &&
    frame->execution_mode == MACHINE_FACADE_EXECUTION_MODE_PROTECTED &&
    (frame->cr0 & 1u) != 0u && (frame->cr0 & 0x80000000u) == 0u &&
    frame->reserved0 == 0u;
}

static void machine_facade_copy_protected_frame_current(
  struct machine_facade_protected_frame *frame)
{
  machine_facade_protected_frame_clear(frame);
  frame->execution_mode = MACHINE_FACADE_EXECUTION_MODE_PROTECTED;
  frame->cr0 = bx_cpu.read_CR0();
  frame->eax = bx_cpu.get_reg32(BX_32BIT_REG_EAX);
  frame->ebx = bx_cpu.get_reg32(BX_32BIT_REG_EBX);
  frame->ecx = bx_cpu.get_reg32(BX_32BIT_REG_ECX);
  frame->edx = bx_cpu.get_reg32(BX_32BIT_REG_EDX);
  frame->esi = bx_cpu.get_reg32(BX_32BIT_REG_ESI);
  frame->edi = bx_cpu.get_reg32(BX_32BIT_REG_EDI);
  frame->ebp = bx_cpu.get_reg32(BX_32BIT_REG_EBP);
  frame->esp = bx_cpu.get_reg32(BX_32BIT_REG_ESP);
  frame->eip = bx_cpu.get_eip();
  frame->eflags = bx_cpu.read_eflags();
  frame->cs = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
  frame->ds = bx_cpu.sregs[BX_SEG_REG_DS].selector.value;
  frame->es = bx_cpu.sregs[BX_SEG_REG_ES].selector.value;
  frame->ss = bx_cpu.sregs[BX_SEG_REG_SS].selector.value;
  frame->fs = bx_cpu.sregs[BX_SEG_REG_FS].selector.value;
  frame->gs = bx_cpu.sregs[BX_SEG_REG_GS].selector.value;
}

extern "C" int machine_facade_copy_protected_frame(
  struct machine_facade_protected_frame *frame)
{
  if (machine_facade_machine == 0 || frame == 0 || !bx_cpu.protected_mode() ||
      bx_cpu.v8086_mode() || (bx_cpu.read_CR0() & 0x80000000u) != 0u)
    return MACHINE_FACADE_PROTECTED_FRAME_REJECTED_MODE;
  machine_facade_copy_protected_frame_current(frame);
  return MACHINE_FACADE_PROTECTED_FRAME_OK;
}

extern "C" int machine_facade_commit_protected_frame(
  const struct machine_facade_protected_frame *expected,
  const struct machine_facade_protected_frame *candidate)
{
  struct machine_facade_protected_frame current;
  uint32_t eflags_changed;
  if (!machine_facade_cpu_paused ||
      !machine_facade_protected_frame_valid(expected) ||
      !machine_facade_protected_frame_valid(candidate))
    return MACHINE_FACADE_PROTECTED_FRAME_REJECTED_INPUT;
  if (machine_facade_copy_protected_frame(&current) !=
      MACHINE_FACADE_PROTECTED_FRAME_OK)
    return MACHINE_FACADE_PROTECTED_FRAME_REJECTED_MODE;
  if (memcmp(&current, expected, sizeof(current)) != 0)
    return MACHINE_FACADE_PROTECTED_FRAME_REJECTED_STALE;
  if (candidate->execution_mode != expected->execution_mode ||
      candidate->cr0 != expected->cr0 || candidate->cs != expected->cs ||
      candidate->ds != expected->ds || candidate->es != expected->es ||
      candidate->ss != expected->ss || candidate->fs != expected->fs ||
      candidate->gs != expected->gs ||
      ((candidate->eflags ^ expected->eflags) &
        ~MACHINE_FACADE_PROTECTED_EFLAGS_WRITE_MASK) != 0u)
    return MACHINE_FACADE_PROTECTED_FRAME_REJECTED_CHANGE;

  /* All validation precedes the first CPU mutation.  The admitted S3 delta
   * deliberately excludes descriptors, CR0 and segment selectors. */
  bx_cpu.set_reg32(BX_32BIT_REG_EAX, candidate->eax);
  bx_cpu.set_reg32(BX_32BIT_REG_EBX, candidate->ebx);
  bx_cpu.set_reg32(BX_32BIT_REG_ECX, candidate->ecx);
  bx_cpu.set_reg32(BX_32BIT_REG_EDX, candidate->edx);
  bx_cpu.set_reg32(BX_32BIT_REG_ESI, candidate->esi);
  bx_cpu.set_reg32(BX_32BIT_REG_EDI, candidate->edi);
  bx_cpu.set_reg32(BX_32BIT_REG_EBP, candidate->ebp);
  bx_cpu.set_reg32(BX_32BIT_REG_ESP, candidate->esp);
  eflags_changed = (candidate->eflags ^ expected->eflags) &
    MACHINE_FACADE_PROTECTED_EFLAGS_WRITE_MASK;
  if (eflags_changed != 0u)
    bx_cpu.writeEFlags(candidate->eflags, eflags_changed);
  bx_cpu.gen_reg[BX_32BIT_REG_EIP].dword.erx = candidate->eip;
  return MACHINE_FACADE_PROTECTED_FRAME_OK;
}

extern "C" int machine_facade_commit_same_cpl_protected_frame(
  const struct machine_facade_protected_frame *expected,
  const struct machine_facade_protected_frame *candidate)
{
  struct machine_facade_protected_frame current;
  bx_cpu_overlay_protected_transition state;
  if (!machine_facade_cpu_paused ||
      !machine_facade_protected_frame_valid(expected) ||
      !machine_facade_protected_frame_valid(candidate))
    return MACHINE_FACADE_PROTECTED_FRAME_REJECTED_INPUT;
  if (machine_facade_copy_protected_frame(&current) !=
      MACHINE_FACADE_PROTECTED_FRAME_OK)
    return MACHINE_FACADE_PROTECTED_FRAME_REJECTED_MODE;
  if (memcmp(&current, expected, sizeof(current)) != 0)
    return MACHINE_FACADE_PROTECTED_FRAME_REJECTED_STALE;
  if (candidate->execution_mode != expected->execution_mode ||
      candidate->cr0 != expected->cr0 ||
      ((candidate->eflags ^ expected->eflags) &
        ~MACHINE_FACADE_PROTECTED_EFLAGS_WRITE_MASK) != 0u)
    return MACHINE_FACADE_PROTECTED_FRAME_REJECTED_CHANGE;
  memset(&state, 0, sizeof(state));
  state.eax = candidate->eax; state.ebx = candidate->ebx;
  state.ecx = candidate->ecx; state.edx = candidate->edx;
  state.esi = candidate->esi; state.edi = candidate->edi;
  state.ebp = candidate->ebp; state.esp = candidate->esp;
  state.eip = candidate->eip; state.eflags = candidate->eflags;
  state.cs = candidate->cs; state.ds = candidate->ds;
  state.es = candidate->es; state.ss = candidate->ss;
  state.fs = candidate->fs; state.gs = candidate->gs;
  return bx_cpu.overlay_commit_same_cpl_protected_transition(&state) ?
    MACHINE_FACADE_PROTECTED_FRAME_OK :
    MACHINE_FACADE_PROTECTED_FRAME_REJECTED_CHANGE;
}

extern "C" void machine_facade_real_mode_frame_clear(
  struct machine_facade_real_mode_frame *frame)
{
  if (frame == 0) return;
  memset(frame, 0, sizeof(*frame));
  frame->abi_version = MACHINE_FACADE_REAL_MODE_FRAME_VERSION;
  frame->struct_bytes = sizeof(*frame);
}

extern "C" int machine_facade_real_mode_frame_valid(
  const struct machine_facade_real_mode_frame *frame)
{
  return frame != 0 &&
    frame->abi_version == MACHINE_FACADE_REAL_MODE_FRAME_VERSION &&
    frame->struct_bytes == sizeof(*frame) &&
    frame->execution_mode == MACHINE_FACADE_EXECUTION_MODE_REAL &&
    (frame->cr0 & 1u) == 0u;
}

static void machine_facade_copy_real_mode_frame_current(
  struct machine_facade_real_mode_frame *frame)
{
  machine_facade_real_mode_frame_clear(frame);
  frame->execution_mode = MACHINE_FACADE_EXECUTION_MODE_REAL;
  frame->cr0 = bx_cpu.read_CR0();
  frame->eflags = bx_cpu.read_eflags();
  frame->cs = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
  frame->ss = bx_cpu.sregs[BX_SEG_REG_SS].selector.value;
  frame->sp = bx_cpu.get_reg16(BX_16BIT_REG_SP);
  frame->ip = (uint16_t)bx_cpu.get_eip();
}

extern "C" int machine_facade_copy_real_mode_frame(
  struct machine_facade_real_mode_frame *frame)
{
  if (machine_facade_machine == 0 || frame == 0 || !bx_cpu.real_mode() ||
      bx_cpu.v8086_mode()) return MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_MODE;
  machine_facade_copy_real_mode_frame_current(frame);
  return MACHINE_FACADE_REAL_MODE_FRAME_OK;
}

extern "C" int machine_facade_commit_real_mode_frame(
  const struct machine_facade_real_mode_frame *expected,
  const struct machine_facade_real_mode_frame *candidate)
{
  struct machine_facade_real_mode_frame current;
  bx_cpu_overlay_real_mode_transition state;

  if (!machine_facade_cpu_paused ||
      !machine_facade_real_mode_frame_valid(expected) ||
      !machine_facade_real_mode_frame_valid(candidate))
    return MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_INPUT;
  if (machine_facade_copy_real_mode_frame(&current) !=
      MACHINE_FACADE_REAL_MODE_FRAME_OK)
    return MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_MODE;
  if (memcmp(&current, expected, sizeof(current)) != 0)
    return MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_STALE;
  if (candidate->execution_mode != expected->execution_mode ||
      candidate->cr0 != expected->cr0 ||
      ((candidate->eflags ^ expected->eflags) &
        ~MACHINE_FACADE_REAL_MODE_EFLAGS_WRITE_MASK) != 0u)
    return MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_CHANGE;
  state.cs = candidate->cs;
  state.ss = candidate->ss;
  state.sp = candidate->sp;
  state.ip = candidate->ip;
  state.eflags = candidate->eflags;
  return bx_cpu.overlay_commit_real_mode_transition(&state) ?
    MACHINE_FACADE_REAL_MODE_FRAME_OK :
    MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_CHANGE;
}

extern "C" int machine_facade_copy_protected_segment(uint32_t slot,
  struct machine_facade_protected_segment *segment)
{
  bx_segment_reg_t const *source;
  if (machine_facade_machine == 0 || segment == 0 || slot >= 6u ||
      !bx_cpu.protected_mode() || bx_cpu.v8086_mode() ||
      (bx_cpu.read_CR0() & 0x80000000u) != 0u)
    return MACHINE_FACADE_PROTECTED_FRAME_REJECTED_MODE;
  source = &bx_cpu.sregs[slot];
  memset(segment, 0, sizeof(*segment));
  segment->abi_version = MACHINE_FACADE_PROTECTED_SEGMENT_VERSION;
  segment->struct_bytes = sizeof(*segment);
  segment->slot = slot;
  segment->base = (uint32_t)source->cache.u.segment.base;
  segment->limit = source->cache.u.segment.limit_scaled;
  segment->access = ((uint32_t)source->cache.valid << 24) |
    ((uint32_t)source->cache.p << 16) | ((uint32_t)source->cache.dpl << 8) |
    (uint32_t)source->cache.type;
  segment->selector = source->selector.value;
  return MACHINE_FACADE_PROTECTED_FRAME_OK;
}

extern "C" int machine_facade_protected_span_transfer(uint32_t kind,
  uint32_t segment, uint32_t offset, uint32_t byte_count, uint8_t *bytes)
{
  return machine_facade_execute_protected_range(kind, segment, offset,
    byte_count, bytes);
}

extern "C" int machine_facade_machine_begin(uint64_t guest_bytes,
  uint64_t host_bytes)
{
  if (machine_facade_machine != 0) return 0;
  machine_facade_machine = new adapter_bochs_minimal_machine_c;
  if (machine_facade_machine == 0 ||
      machine_facade_machine->initialize((Bit64u)guest_bytes,
        (Bit64u)host_bytes) != BX_MACHINE_MINIMAL_MACHINE_OK) {
    delete machine_facade_machine;
    machine_facade_machine = 0;
    return 0;
  }
  machine_facade_cpu_paused = 1;
  return 1;
}

extern "C" int machine_facade_machine_cleanup(void)
{
  adapter_bochs_minimal_machine_c *machine = machine_facade_machine;
  machine_facade_machine = 0;
  machine_facade_cpu_paused = 0;
  if (machine == 0) return 1;
  if (machine->cleanup() != BX_MACHINE_MINIMAL_MACHINE_OK) {
    delete machine;
    return 0;
  }
  delete machine;
  return 1;
}

extern "C" int machine_facade_machine_active(void)
{
  return machine_facade_machine != 0;
}

extern "C" int machine_facade_set_realmode_segment_limit_compatibility(
  uint32_t enabled)
{
  return machine_facade_machine != 0 && enabled <= 1u &&
    machine_facade_machine->set_realmode_segment_limit_compatibility(
      enabled ? 1 : 0) ? 1 : 0;
}

extern "C" int machine_facade_compose_headless_8042(void)
{
  return machine_facade_machine != 0 &&
    machine_facade_machine->compose_headless_8042() ==
      BX_MACHINE_MINIMAL_MACHINE_OK ? 1 : 0;
}

extern "C" void machine_facade_initialize_timing(uint32_t ips)
{
  bx_pc_system.initialize((unsigned)ips);
}

extern "C" void machine_facade_apply_real_mode_entry(uint16_t cs,
  uint32_t eip)
{
  bx_cpu.apply_real_mode_entry(cs, eip);
}

extern "C" int machine_facade_copy_real_mode_entry(uint16_t *cs,
  uint32_t *eip)
{
  if (cs == 0 || eip == 0) return 0;
  *cs = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
  *eip = bx_cpu.get_eip();
  return 1;
}

extern "C" int machine_facade_copy_real_mode_state(uint16_t *cs,
  uint16_t *ss, uint16_t *sp, uint32_t *eip)
{
  if (cs == 0 || ss == 0 || sp == 0 || eip == 0) return 0;
  *cs = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
  *ss = bx_cpu.sregs[BX_SEG_REG_SS].selector.value;
  *sp = bx_cpu.get_reg16(BX_16BIT_REG_SP);
  *eip = bx_cpu.get_eip();
  return 1;
}

extern "C" int machine_facade_copy_ax16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.get_reg16(BX_16BIT_REG_AX);
  return 1;
}

extern "C" int machine_facade_copy_bx16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.get_reg16(BX_16BIT_REG_BX);
  return 1;
}

extern "C" int machine_facade_copy_cx16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.get_reg16(BX_16BIT_REG_CX);
  return 1;
}

extern "C" int machine_facade_copy_dx16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.get_reg16(BX_16BIT_REG_DX);
  return 1;
}

extern "C" int machine_facade_copy_bp16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.get_reg16(BX_16BIT_REG_BP);
  return 1;
}

extern "C" int machine_facade_copy_sp16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.get_reg16(BX_16BIT_REG_SP);
  return 1;
}

extern "C" int machine_facade_copy_di16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.get_reg16(BX_16BIT_REG_DI);
  return 1;
}

extern "C" int machine_facade_copy_ip16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = (uint16_t)bx_cpu.get_eip();
  return 1;
}

extern "C" int machine_facade_copy_cs16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
  return 1;
}

extern "C" int machine_facade_copy_ss16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.sregs[BX_SEG_REG_SS].selector.value;
  return 1;
}

extern "C" int machine_facade_copy_si16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.get_reg16(BX_16BIT_REG_SI);
  return 1;
}

extern "C" int machine_facade_copy_ds16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.sregs[BX_SEG_REG_DS].selector.value;
  return 1;
}

extern "C" int machine_facade_copy_es16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.sregs[BX_SEG_REG_ES].selector.value;
  return 1;
}

extern "C" int machine_facade_copy_fs16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.sregs[BX_SEG_REG_FS].selector.value;
  return 1;
}

extern "C" int machine_facade_copy_gs16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.sregs[BX_SEG_REG_GS].selector.value;
  return 1;
}

extern "C" int machine_facade_copy_msw16(uint16_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = (uint16_t)bx_cpu.cr0.get32();
  return 1;
}

#define MACHINE_FACADE_COPY_REGISTER32(name, reg) \
extern "C" int machine_facade_copy_##name##32(uint32_t *value) \
{ \
  if (machine_facade_machine == 0 || value == 0) return 0; \
  *value = bx_cpu.get_reg32(reg); \
  return 1; \
}

MACHINE_FACADE_COPY_REGISTER32(eax, BX_32BIT_REG_EAX)
MACHINE_FACADE_COPY_REGISTER32(ebx, BX_32BIT_REG_EBX)
MACHINE_FACADE_COPY_REGISTER32(ecx, BX_32BIT_REG_ECX)
MACHINE_FACADE_COPY_REGISTER32(edx, BX_32BIT_REG_EDX)
MACHINE_FACADE_COPY_REGISTER32(esi, BX_32BIT_REG_ESI)
MACHINE_FACADE_COPY_REGISTER32(edi, BX_32BIT_REG_EDI)
MACHINE_FACADE_COPY_REGISTER32(ebp, BX_32BIT_REG_EBP)
MACHINE_FACADE_COPY_REGISTER32(esp, BX_32BIT_REG_ESP)

extern "C" int machine_facade_copy_eip32(uint32_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.get_eip();
  return 1;
}

extern "C" int machine_facade_copy_al8(uint8_t *value)
{
  if (machine_facade_machine == 0 || value == 0) return 0;
  *value = bx_cpu.get_reg8l(BX_16BIT_REG_AX);
  return 1;
}

extern "C" int machine_facade_set_al8(uint8_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.set_reg8l(BX_16BIT_REG_AX, value);
  return 1;
}

extern "C" int machine_facade_set_bl8(uint8_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.set_reg8l(BX_16BIT_REG_BX, value);
  return 1;
}

extern "C" int machine_facade_set_ax16(uint16_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.set_reg16(BX_16BIT_REG_AX, value);
  return 1;
}

extern "C" int machine_facade_set_bx16(uint16_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.set_reg16(BX_16BIT_REG_BX, value);
  return 1;
}

extern "C" int machine_facade_set_dx16(uint16_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.set_reg16(BX_16BIT_REG_DX, value);
  return 1;
}

extern "C" int machine_facade_set_cx16(uint16_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.set_reg16(BX_16BIT_REG_CX, value);
  return 1;
}

extern "C" int machine_facade_set_bp16(uint16_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.set_reg16(BX_16BIT_REG_BP, value);
  return 1;
}

extern "C" int machine_facade_set_sp16(uint16_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.set_reg16(BX_16BIT_REG_SP, value);
  return 1;
}

extern "C" int machine_facade_set_si16(uint16_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.set_reg16(BX_16BIT_REG_SI, value);
  return 1;
}

extern "C" int machine_facade_set_di16(uint16_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.set_reg16(BX_16BIT_REG_DI, value);
  return 1;
}

extern "C" int machine_facade_set_ip16(uint16_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.gen_reg[BX_32BIT_REG_EIP].word.rx = value;
  return 1;
}

#define MACHINE_FACADE_SET_REGISTER32(name, reg) \
extern "C" int machine_facade_set_##name##32(uint32_t value) \
{ \
  if (machine_facade_machine == 0) return 0; \
  bx_cpu.set_reg32(reg, value); \
  return 1; \
}

MACHINE_FACADE_SET_REGISTER32(eax, BX_32BIT_REG_EAX)
MACHINE_FACADE_SET_REGISTER32(ebx, BX_32BIT_REG_EBX)
MACHINE_FACADE_SET_REGISTER32(ecx, BX_32BIT_REG_ECX)
MACHINE_FACADE_SET_REGISTER32(edx, BX_32BIT_REG_EDX)
MACHINE_FACADE_SET_REGISTER32(esi, BX_32BIT_REG_ESI)
MACHINE_FACADE_SET_REGISTER32(edi, BX_32BIT_REG_EDI)
MACHINE_FACADE_SET_REGISTER32(ebp, BX_32BIT_REG_EBP)
MACHINE_FACADE_SET_REGISTER32(esp, BX_32BIT_REG_ESP)

extern "C" int machine_facade_set_eip32(uint32_t value)
{
  if (machine_facade_machine == 0) return 0;
  bx_cpu.gen_reg[BX_32BIT_REG_EIP].dword.erx = value;
  return 1;
}

extern "C" int machine_facade_set_carry(uint32_t value)
{
  if (machine_facade_machine == 0 || value > 1u) return 0;
  bx_cpu.set_CF(value ? 1 : 0);
  return 1;
}

extern "C" void machine_facade_request_cpu_stop(void)
{
  bx_pc_system.kill_bochs_request = 1;
  bx_cpu.async_event = 1;
}

extern "C" int machine_facade_prepare_cpu_resume(void)
{
  if (machine_facade_machine == 0) return 0;
  /* The finite-run timer is adapter-owned.  CPU event handling clears the
   * asynchronous indication after this stop latch is released. */
  bx_pc_system.kill_bochs_request = 0;
  machine_facade_cpu_paused = 0;
  return 1;
}

extern "C" void machine_facade_cpu_loop(void)
{
  machine_facade_cpu_paused = 0;
  bx_cpu.cpu_loop();
  machine_facade_cpu_paused = 1;
}

extern "C" int machine_facade_register_timer(void *opaque,
  machine_facade_timer_callback callback, uint64_t ticks,
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

extern "C" void machine_facade_deactivate_timer(uint32_t timer_id)
{
  bx_pc_system.deactivate_timer((unsigned)timer_id);
}

extern "C" void machine_facade_unregister_timer(uint32_t timer_id)
{
  bx_pc_system.unregisterTimer((unsigned)timer_id);
}
