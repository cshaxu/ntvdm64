/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-DPMI-MECH-001): private selector-blind protected CPU-frame
// transaction. It is callable only through the bochs-core declaration.
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "descriptor.h"

#define LOG_THIS BX_CPU_THIS_PTR

static bx_bool overlay_fetch_descriptor(BX_CPU_C *cpu, Bit16u raw,
  bx_selector_t *selector, bx_descriptor_t *descriptor)
{
  Bit32u low, high;
  parse_selector(raw, selector);
  if (!cpu->fetch_raw_descriptor2(selector, &low, &high)) return 0;
  parse_descriptor(low, high, descriptor);
  return descriptor->valid;
}

static bx_bool overlay_validate_code(BX_CPU_C *cpu, Bit16u raw, Bit32u eip,
  bx_selector_t *selector, bx_descriptor_t *descriptor)
{
  if ((raw & 0xfffc) == 0 || !overlay_fetch_descriptor(cpu, raw, selector,
      descriptor) || descriptor->segment == 0 ||
      IS_DATA_SEGMENT(descriptor->type) || !descriptor->p ||
      eip > descriptor->u.segment.limit_scaled)
    return 0;
  if (IS_CODE_SEGMENT_NON_CONFORMING(descriptor->type))
    return descriptor->dpl == selector->rpl && selector->rpl ==
      cpu->sregs[BX_SEG_REG_CS].selector.rpl;
  return descriptor->dpl <= cpu->sregs[BX_SEG_REG_CS].selector.rpl;
}

static bx_bool overlay_validate_ss(BX_CPU_C *cpu, Bit16u raw,
  bx_selector_t *selector, bx_descriptor_t *descriptor)
{
  Bit8u cpl = cpu->sregs[BX_SEG_REG_CS].selector.rpl;
  return (raw & 0xfffc) != 0 && overlay_fetch_descriptor(cpu, raw, selector,
    descriptor) && descriptor->segment && !IS_CODE_SEGMENT(descriptor->type) &&
    IS_DATA_SEGMENT_WRITEABLE(descriptor->type) && descriptor->p &&
    selector->rpl == cpl && descriptor->dpl == cpl;
}

static bx_bool overlay_validate_data(BX_CPU_C *cpu, Bit16u raw,
  bx_selector_t *selector, bx_descriptor_t *descriptor)
{
  Bit8u cpl = cpu->sregs[BX_SEG_REG_CS].selector.rpl;
  if ((raw & 0xfffc) == 0) return 1;
  if (!overlay_fetch_descriptor(cpu, raw, selector, descriptor) ||
      descriptor->segment == 0 ||
      (IS_CODE_SEGMENT(descriptor->type) &&
       !IS_CODE_SEGMENT_READABLE(descriptor->type)) || !descriptor->p)
    return 0;
  if (IS_DATA_SEGMENT(descriptor->type) ||
      IS_CODE_SEGMENT_NON_CONFORMING(descriptor->type))
    return selector->rpl <= descriptor->dpl && cpl <= descriptor->dpl;
  return 1;
}

static void overlay_load_data(BX_CPU_C *cpu, unsigned slot, Bit16u raw,
  bx_selector_t *selector, bx_descriptor_t *descriptor)
{
  if ((raw & 0xfffc) == 0) {
    cpu->load_null_selector(&cpu->sregs[slot], raw);
    return;
  }
  cpu->touch_segment(selector, descriptor);
  cpu->sregs[slot].selector = *selector;
  cpu->sregs[slot].cache = *descriptor;
  cpu->sregs[slot].cache.valid = 1;
}

int BX_CPU_C::overlay_commit_same_cpl_protected_transition(
  const bx_cpu_overlay_protected_transition *state)
{
  bx_selector_t cs, ss, ds, es, fs, gs;
  bx_descriptor_t csd, ssd, dsd, esd, fsd, gsd;
  Bit32u changed;
  if (state == 0 || !protected_mode() || v8086_mode() || long_mode() ||
      (read_CR0() & 0x80000000u) != 0u ||
      !overlay_validate_code(this, state->cs, state->eip, &cs, &csd) ||
      !overlay_validate_ss(this, state->ss, &ss, &ssd) ||
      !overlay_validate_data(this, state->ds, &ds, &dsd) ||
      !overlay_validate_data(this, state->es, &es, &esd) ||
      !overlay_validate_data(this, state->fs, &fs, &fsd) ||
      !overlay_validate_data(this, state->gs, &gs, &gsd))
    return 0;

  /* All validity checks precede the first architectural mutation. */
  overlay_load_data(this, BX_SEG_REG_DS, state->ds, &ds, &dsd);
  overlay_load_data(this, BX_SEG_REG_ES, state->es, &es, &esd);
  overlay_load_data(this, BX_SEG_REG_FS, state->fs, &fs, &fsd);
  overlay_load_data(this, BX_SEG_REG_GS, state->gs, &gs, &gsd);
  load_ss(&ss, &ssd, csd.dpl);
  load_cs(&cs, &csd, csd.dpl);
  EAX = state->eax; EBX = state->ebx; ECX = state->ecx; EDX = state->edx;
  ESI = state->esi; EDI = state->edi; EBP = state->ebp; ESP = state->esp;
  changed = read_eflags() ^ state->eflags;
  if (changed != 0) writeEFlags(state->eflags, changed);
  EIP = state->eip;
  return 1;
}
