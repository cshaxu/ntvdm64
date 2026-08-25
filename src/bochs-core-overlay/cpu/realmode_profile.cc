/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-CORE-DIV-001,BX-EXEC-016): private real/V86 compatibility
// profile and entry implementation; declarations remain in bochs-core.
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

void BX_CPU_C::overlay_initialize_realmode_profile(void)
{
  realmode_segment_limit_compatibility = 0;
}

void BX_CPU_C::set_realmode_segment_limit_compatibility(bx_bool enabled)
{
  realmode_segment_limit_compatibility = enabled ? 1 : 0;
}

void BX_CPU_C::apply_real_mode_state(Bit32u eax, Bit32u ebx, Bit32u ecx, Bit32u edx,
  Bit32u esi, Bit32u edi, Bit32u ebp, Bit32u esp, Bit32u eip, Bit32u eflags,
  Bit16u cs, Bit16u ds, Bit16u es, Bit16u ss, Bit16u fs, Bit16u gs)
{
  EAX=eax; EBX=ebx; ECX=ecx; EDX=edx; ESI=esi; EDI=edi; EBP=ebp; ESP=esp;
  load_seg_reg(&sregs[BX_SEG_REG_CS], cs); load_seg_reg(&sregs[BX_SEG_REG_DS], ds);
  load_seg_reg(&sregs[BX_SEG_REG_ES], es); load_seg_reg(&sregs[BX_SEG_REG_SS], ss);
  load_seg_reg(&sregs[BX_SEG_REG_FS], fs); load_seg_reg(&sregs[BX_SEG_REG_GS], gs);
  RIP=eip; setEFlags(eflags);
}

void BX_CPU_C::apply_real_mode_entry(Bit16u cs, Bit32u eip)
{
  load_seg_reg(&sregs[BX_SEG_REG_CS], cs);
  RIP=eip;
  invalidate_prefetch_q();
}
